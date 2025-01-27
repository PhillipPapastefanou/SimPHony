from flask import Flask, render_template
from bokeh.embed import file_html
from bokeh.resources import CDN
import geopandas as gpd
import pandas as pd
import numpy as np
import hvplot.pandas
from shapely.geometry import Polygon
from pyproj import CRS, Transformer
import holoviews as hv
import geoviews as gv
import cartopy.crs as ccrs

app = Flask(__name__)

def create_tiles(num_tiles=1000, tile_size_km=100):
    """Creates GeoDataFrame of random tiles."""
    np.random.seed(0)  # for reproducibility
    center_lats = np.random.uniform(-90, 90, num_tiles)
    center_lons = np.random.uniform(-180, 180, num_tiles)

    polygons = []
    for lat, lon in zip(center_lats, center_lons):
        lat_deg = tile_size_km * 0.5 / 111
        lon_deg = tile_size_km * 0.5 / (111 * np.cos(np.deg2rad(lat)))

        polygon = Polygon([
            (lon - lon_deg, lat - lat_deg),
            (lon + lon_deg, lat - lat_deg),
            (lon + lon_deg, lat + lat_deg),
            (lon - lon_deg, lat + lat_deg),
        ])
        polygons.append(polygon)

    gdf = gpd.GeoDataFrame({'geometry': polygons}, crs="EPSG:4326")
    return gdf

def create_map(gdf):
    transformer = Transformer.from_crs(CRS("EPSG:4326"), CRS("EPSG:3857"), always_xy=True)
    gdf['x'] = gdf.geometry.centroid.x.apply(lambda x: transformer.transform(x, 0)[0])
    gdf['y'] = gdf.geometry.centroid.y.apply(lambda x: transformer.transform(0, x)[1])
    gdf['value'] = np.random.rand(len(gdf))

    # Convert to GeoViews Polygons directly
    geodf = gv.Polygons(gdf, crs=ccrs.GOOGLE_MERCATOR)

    # Use a pre-defined tile source from GeoViews
    tiles = gv.tile_sources.StamenTerrain

    # Overlay tiles and GeoDataFrame
    plot = tiles * geodf.opts(width=800, height=600, alpha=0.2, line_color="black", line_width=0.5, color='value', cmap='viridis')

    return plot

@app.route('/')
def index():
    gdf = create_tiles(num_tiles=1000)
    plot = create_map(gdf)

    # Render to Bokeh
    bokeh_plot = hv.render(plot, backend='bokeh')

    models = [bokeh_plot]
    html = file_html(models, CDN, "my_map")
    return render_template('index.html', bokeh_html=html)

if __name__ == '__main__':
    app.run(debug=True)