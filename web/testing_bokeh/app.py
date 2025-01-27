from flask import Flask, render_template
from bokeh.embed import file_html
from bokeh.plotting import figure
from bokeh.resources import CDN
from bokeh.models import WMTSTileSource
import pandas as pd
from pyproj import Transformer

app = Flask(__name__)

# Sample data (replace with your own)
data = {'latitude': [40.7128, 34.0522, 37.7749, 48.8566, 51.5074],
        'longitude': [-74.0060, -118.2437, -122.4194, 2.3522, 0.1278],
        'temperature': [15, 22, 18, 10, 8]}
df = pd.DataFrame(data)
import pandas as pd
import datashader as ds
import datashader.transfer_functions as tf
from bokeh.plotting import figure, show
from bokeh.models import WMTSTileSource
from pyproj import Transformer
import numpy as np

# Sample data (replace with your actual data)
np.random.seed(0)
n = 10000
data = {'latitude': np.random.uniform(-90, 90, n),
        'longitude': np.random.uniform(-180, 180, n),
        'value': np.random.rand(n)}
df = pd.DataFrame(data)


def create_map():
    # Transform coordinates
    transformer = Transformer.from_crs("epsg:4326", "epsg:3857")
    x, y = transformer.transform(df['latitude'].values, df['longitude'].values)
    df['x'] = x
    df['y'] = y

    # Create Datashader canvas
    cvs = ds.Canvas(plot_width=800, plot_height=600, x_range=(-20000000, 20000000), y_range=(-20000000, 20000000))
    agg = cvs.points(df, 'x', 'y', agg=ds.mean('value')) # aggregate by mean of 'value'

    # Create an image from the aggregate
    img = tf.shade(agg, cmap=['blue', 'green', 'yellow', 'red'])

    # Get the tile source
    url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'
    source = WMTSTileSource(url=url)

    # Create the Bokeh figure
    p = figure(x_range=(-20000000, 20000000), y_range=(-20000000, 20000000),
               x_axis_type="mercator", y_axis_type="mercator",
               width=800, height=600, title="Datashaded Map")

    p.add_tile(source)

    # Plot the image
    p.image_rgba(image=[img.data], x=-20000000, y=-20000000, dw=40000000, dh=40000000)

    return p
# def create_map():
#     # Tile source (Stamen Terrain - requires attribution)
#     url = 'https://tiles.stadiamaps.com/tiles/stamen_terrain/{z}/{x}/{y}{r}.png'  # Or another URL
#     source = WMTSTileSource(url=url)
#
#     p = figure(x_range=(-20000000, 20000000), y_range=(-20000000, 20000000),
#                x_axis_type="mercator", y_axis_type="mercator",
#                width=800, height=600, title="Test Map")
#
#     p.add_tile(source)
#
#     html = file_html(p, CDN, "test_map")
#     return render_template('index.html', bokeh_html=html)
@app.route('/')
def index():


    plot = create_map()  # Call create_map() here
    models = [plot] # wrap in list
    html = file_html(models, CDN, "my_map")
    return render_template('index.html', bokeh_html=html)
    # url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'
    # source = WMTSTileSource(url=url)
    #
    # p = figure(x_range=(-20000000, 20000000), y_range=(-20000000, 20000000),
    #            x_axis_type="mercator", y_axis_type="mercator",
    #            width=800, height=600, title="Test Map")
    #
    # p.add_tile(source)
    #
    # # Wrap the figure in a list
    # models = [p]  # This is the fix
    #
    # html = file_html(models, CDN, "test_map")  # Pass the list of models
    # return render_template('index.html', bokeh_html=html)

if __name__ == '__main__':
    app.run(debug=True)