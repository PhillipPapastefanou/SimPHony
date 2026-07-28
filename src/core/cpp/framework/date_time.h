//
// Created by Phillip on 10.03.24.
//
#pragma once
#include <string>
#include <ctime>  // Include this header for the tm structure


class DateTime {

public:
    DateTime();
    DateTime(tm tms);
    DateTime(std::string date_string, std::string format);
    DateTime(const DateTime& dateTime, long seconds);
    explicit DateTime(time_t epoch_seconds);
    DateTime AddSeconds(long seconds);
    ~DateTime();
    void print();

    int year;
    int month;
    int day;

    int hour;
    int min;
    int sec;

    tm tmt;
    time_t t;

private:
    // Fixed CET offset (UTC+1, no daylight saving) applied when deriving the
    // year/month/day/hour/min/sec fields from `t`. All forcing data is
    // timestamped in true UTC; the model and dashboard need those fields in
    // Central European time. A fixed offset (rather than real CET/CEST with
    // DST transitions) avoids an artificial 1-hour jump appearing mid-run
    // purely from a calendar rule. `t` itself always stays true UTC epoch
    // seconds, so arithmetic/comparisons/AddSeconds are unaffected.
    static constexpr long CET_OFFSET_SECONDS = 3600;
    void create_time();
    void populate_fields();
};

long operator -(DateTime t1, DateTime t2);
bool operator < (DateTime t1, DateTime t2);
bool operator > (DateTime t1, DateTime t2);
bool operator == (DateTime t1, DateTime t2);
bool operator != (DateTime t1, DateTime t2);