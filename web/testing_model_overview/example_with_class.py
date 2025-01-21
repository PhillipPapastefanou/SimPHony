from flask import Flask, render_template, request, jsonify
import plotly.graph_objects as go
import plotly.offline as py
import numpy as np
import pandas as pd
import json

class Web_Interface:
    def __init__(self):

        # Define the start date
        self.start_date = pd.to_datetime('2024-07-05')  # Example start date

        # Create an array of 100 hourly time intervals (in seconds)
        self.time_intervals = pd.to_timedelta(np.arange(100) * 60 * 60, unit='s')

        # Create the x-axis range by adding the time intervals to the start date
        self.x_range = self.start_date + self.time_intervals
        self.x = np.arange(0,100, 0.1)

    def run_model(self, value_1, value_2):
        self.y1 = np.sin(value_1 * self.x) * value_2
        self.y2 = np.cos(value_1 * self.x) * value_2 + value_2 * self.x
app = Flask(__name__)
web_interface = Web_Interface()

@app.route('/')
def index():
    return render_template('index_style.html')

@app.route('/update_plot', methods=['POST'])
def update_plot():

    try:
        slider1_value = float(request.form['slider1'])
        slider2_value = float(request.form['slider2'])
    except (ValueError, KeyError):
        return jsonify({'error': 'Invalid slider values'}), 400



    # Main model run routine

    web_interface.run_model(slider1_value, slider2_value)


    fig1 = go.Figure(data=[
        go.Scatter(x=web_interface.x, y=web_interface.y1, name = 'a'),
        go.Scatter(x=web_interface.x, y=web_interface.y1, name = "b")
    ])

    fig1.update_layout(
        xaxis_title="Time",  # X-axis title
        yaxis_title="Water potential" , # Y-axis title
        yaxis=dict(
            range=[-5, 5]  # Set y-axis limits here
        )
    )

    fig2 = go.Figure(data=[go.Scatter(x=web_interface.x_range, y=web_interface.y2)])
    fig2.update_layout(
        xaxis_title="Time",  # X-axis title
        yaxis_title="Stomatal conductance"   # Y-axis title
    )



    plot_json1 = json.loads(fig1.to_json())  # Parse the JSON string to a Python dict
    plot_json2 = json.loads(fig2.to_json())

    return jsonify({'plot1': plot_json1, 'plot2': plot_json2})  # Send the parsed JSON object

if __name__ == '__main__':
    app.run(debug=True)