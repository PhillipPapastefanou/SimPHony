#!/usr/bin/env python3
"""
Convert the Hainich forcing-data CSV into a compact binary blob (FRC3) that
the WASM app can load with a single fetch + memcpy, with zero per-row
parsing at runtime.

No command-line arguments - just edit INPUT_CSV / OUTPUT_BIN below and hit
Run in your IDE.

Expected CSV header (fixed, not configurable):
  datetime,Ta_4400,VPD_4400,P_4400,Pa_4400,PPFD_IN_4400,SWDR_4400,SM_08,SM_16,SM_32
  2023-01-01T00:00:00Z,13.96,0.7909173813872399,0,956.869,0.849,0,36.28125,33.41,44.98

Binary layout (all little-endian):

  Header (16 bytes):
    char[4]  magic       = b"FRC3"
    int32    n           = number of timesteps
    int32    n_layers    = number of theta (soil moisture) layers (3)
    int32    reserved    = 0

  Body (n*4 + n*4 + n*4 + n*4 + n*n_layers*4 + n*4 bytes), contiguous, no padding:
    int32    timestamps[n]      unix seconds (UTC)
    float32  vpd[n]             raw units, same as CSV (C++ scales *1000)
    float32  rad[n]             raw units
    float32  temp[n]            deg C, raw (matches Output/temp_air convention)
    float32  theta[n*n_layers]  row-major: theta[i*n_layers + layer], raw
                                 (0-100 scale; C++ divides by 100)
    float32  precip[n]          precipitation RATE [kg m-2 s-1] == [mm s-1] -- P_4400 is a
                                 depth [mm] accumulated over one forcing timestep, converted
                                 here (divided by the timestep length in seconds) so the C++
                                 side (Input_Hainich::Set_Forcing_Data_Blob) can use it
                                 directly, matching the convention used by the CSV-parsing
                                 path (Input_Hainich::Read_N_Parse) and the Swiss "rainf"
                                 forcing column (already a rate).

FRC3 adds the trailing precip[n] block on top of FRC2 -- everything else is unchanged.
Only used by the prognostic soil hydrology option (Parameters::use_prognostic_soil_hydrology);
harmless/unused otherwise.
"""

import csv
import struct
import sys
from datetime import datetime, timezone

# ---------------------------------------------------------------------------
# Edit these two, then run this file.
# ---------------------------------------------------------------------------
INPUT_CSV = "/Users/pp/Documents/Repos/SimPHony/data/hainich/input/Meteo_Hainich_dT30min_forcing_PHS.csv"
OUTPUT_BIN = "forcing.bin"

# ---------------------------------------------------------------------------
# Fixed column mapping for the Hainich forcing CSV - not configurable.
# ---------------------------------------------------------------------------
DATETIME_COL = "datetime"
DATETIME_FORMAT = "%Y-%m-%dT%H:%M:%SZ"
TEMP_COL = "Ta_4400"
VPD_COL = "VPD_4400"
RAD_COL = "SWDR_4400"
PRECIP_COL = "P_4400"
THETA_COLS = ["SM_08", "SM_16", "SM_32"]
MAGIC = b"FRC3"


def convert(input_csv: str, output_bin: str) -> None:
    n_layers = len(THETA_COLS)

    timestamps = []
    vpd = []
    rad = []
    temp = []
    theta = []  # flat, row-major
    precip_mm = []  # raw depth [mm] per timestep, converted to a rate below

    with open(input_csv, newline="") as f:
        reader = csv.DictReader(f)
        required = [DATETIME_COL, TEMP_COL, VPD_COL, RAD_COL, PRECIP_COL] + THETA_COLS
        missing = [c for c in required if c not in reader.fieldnames]
        if missing:
            sys.exit(f"error: columns not found in CSV header: {missing}\n"
                      f"available columns: {reader.fieldnames}")

        for row_num, row in enumerate(reader, start=2):
            try:
                # The project's DateTime class always builds epoch seconds via
                # timegm() (see date_time.cpp), i.e. the parsed wall-clock fields
                # are treated as UTC with no DST/timezone adjustment. Match that
                # exactly here so a round trip through this binary reproduces the
                # same `t` the C++ side would get from parsing the CSV string
                # directly.
                dt = datetime.strptime(row[DATETIME_COL], DATETIME_FORMAT)
                ts = int(dt.replace(tzinfo=timezone.utc).timestamp())

                timestamps.append(ts)
                vpd.append(float(row[VPD_COL]))
                rad.append(float(row[RAD_COL]))
                temp.append(float(row[TEMP_COL]))
                precip_mm.append(float(row[PRECIP_COL]))
                for c in THETA_COLS:
                    theta.append(float(row[c]))
            except ValueError as e:
                sys.exit(f"error parsing row {row_num}: {e}")

    n = len(timestamps)
    if n == 0:
        sys.exit("error: no rows parsed")

    # Convert precipitation from a depth [mm] accumulated over one timestep to a rate
    # [kg m-2 s-1] == [mm s-1], using the actual spacing between consecutive timestamps
    # (constant for this regular-interval forcing) rather than assuming 1800s.
    dt_seconds = timestamps[1] - timestamps[0] if n > 1 else 1800
    precip = [p / dt_seconds for p in precip_mm]

    with open(output_bin, "wb") as f:
        f.write(MAGIC)
        f.write(struct.pack("<iii", n, n_layers, 0))
        f.write(struct.pack(f"<{n}i", *timestamps))
        f.write(struct.pack(f"<{n}f", *vpd))
        f.write(struct.pack(f"<{n}f", *rad))
        f.write(struct.pack(f"<{n}f", *temp))
        f.write(struct.pack(f"<{n * n_layers}f", *theta))
        f.write(struct.pack(f"<{n}f", *precip))

    total_bytes = 16 + n * 4 + n * 4 + n * 4 + n * 4 + n * n_layers * 4 + n * 4
    print(f"wrote {output_bin}: n={n}, n_layers={n_layers}, dt={dt_seconds}s, "
          f"{total_bytes} bytes ({total_bytes/1024:.1f} KB)")


def verify(path: str) -> None:
    with open(path, "rb") as f:
        data = f.read()

    magic = data[0:4]
    if magic != MAGIC:
        sys.exit(f"error: bad magic {magic!r}, expected {MAGIC!r}")

    n, n_layers, reserved = struct.unpack_from("<iii", data, 4)
    off = 16
    timestamps = struct.unpack_from(f"<{n}i", data, off); off += n * 4
    vpd = struct.unpack_from(f"<{n}f", data, off); off += n * 4
    rad = struct.unpack_from(f"<{n}f", data, off); off += n * 4
    temp = struct.unpack_from(f"<{n}f", data, off); off += n * 4
    theta = struct.unpack_from(f"<{n * n_layers}f", data, off); off += n * n_layers * 4
    precip = struct.unpack_from(f"<{n}f", data, off); off += n * 4

    expected_size = off
    actual_size = len(data)

    print(f"verify: magic={magic}, n={n}, n_layers={n_layers}")
    print(f"verify: expected size={expected_size}, actual size={actual_size}, "
          f"{'OK' if expected_size == actual_size else 'MISMATCH'}")
    print(f"verify: first timestamp={timestamps[0]} ({datetime.utcfromtimestamp(timestamps[0])})")
    print(f"verify: first vpd={vpd[0]}, first rad={rad[0]}, first temp={temp[0]}")
    print(f"verify: first theta row={theta[0:n_layers]}")
    print(f"verify: precip rate range=[{min(precip)}, {max(precip)}] kg m-2 s-1, "
          f"sum(precip)*dt~{sum(precip) * (timestamps[1]-timestamps[0]):.1f} mm total")
    print(f"verify: last timestamp={timestamps[-1]} ({datetime.utcfromtimestamp(timestamps[-1])})")


if __name__ == "__main__":
    convert(INPUT_CSV, OUTPUT_BIN)
    verify(OUTPUT_BIN)