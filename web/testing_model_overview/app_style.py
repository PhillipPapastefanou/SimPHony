from flask import Flask, render_template, request, jsonify
import plotly.graph_objects as go
import plotly.offline as py
import numpy as np
import pandas as pd
import json

import sys
import os
import copy
from time import perf_counter

import numpy as np
import pandas as pd
import datetime
import time

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.auxil.output_df import create_output_df
from src.contrib.auxil.output_plotter import std_plot, eval_plot_24
found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
sys.path.append(cpp_lib_path)

# Importing local libraries and paths
from SimPHony import Simulation_Single_Hainich
from SimPHony import DateTime

from src.core.py.Parameters import Parameters
from src.core.py.Parameters import SoilLayer
from src.core.py.Parameters import Soil_Water_Model_Type
from src.core.py.Parameters import Convert_Soil_Parameters
from src.contrib.config import Config, Location
from src.contrib.auxil.setups import Setup

root_path = THIS_DIR

class Web_Interface:
    def __init__(self):

        scenario = "sim"

        self.setup = Setup()
        self.setup.Apply_default_hainich()
        self.setup.Apply_default_paths(root_path, scenario)
        # setup.config.parameters_list_file = os.path.join(setup.config.post_path, "parameters_best_mean_alive.csv")
        self.setup.config.config_file = os.path.join(self.setup.config.input_path, "config_py.txt")
        self.setup.Export()

    def run_model(self, k_xylem, gamma_stem, psi_leaf_50_close, gamma_leaf):

        # Create parameters
        params = Parameters()

        nsoil_layers = 3
        layer = SoilLayer()
        layer.k_soil_sat = 1.0 / 100.0 / 86400.0
        layer.psi_soil_sat = -0.5 * 1
        # layer.camp_b  = 10.4
        layer.theta_s = 0.48
        layer.theta_r = 0.05
        layer.pore_size_ind = 0.6

        # Copy the soil layer and assume all layers have the same properties...
        soil_layers = []
        for s in range(nsoil_layers):
            soil_layers.append(copy.deepcopy(layer))

        # ... but not the depth
        soil_layers[0].depth = 0.08
        soil_layers[1].depth = 0.16
        soil_layers[2].depth = 0.32

        Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

        params.canopy_height = 31
        params.huber_value = 1.0 / 3000.0
        params.k_xylem_sat = k_xylem * 1000 / 18
        params.stem_hydraulic_capacitance = gamma_stem * 1000 / 18
        params.leaf_hydraulic_capacitance = gamma_leaf/1000 * 1000 / 18
        params.g_bark = 0.06
        params.g0 = 0.02
        params.g1 = 1.5
        params.leaf_area_index = 4.8
        params.psi_leaf_50_close = psi_leaf_50_close
        params.d_50_close = 2.0
        params.psi50_xylem = -3.5
        params.psi88_xylem = -5.5
        params.root_area_index = 4.5
        params.jackson_root_beta = 0.96
        params.tree_density = 64.0 / 10000.0
        params.anet_max = 2.5
        params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
        params.sw_rad_max = 1040

        cparameters = params.Create_CParameters(soil_layers=soil_layers)

        # ----------------------------------------------------------------
        # SimPHony model simulation
        # ----------------------------------------------------------------

        # Setting up the simulation
        sim = Simulation_Single_Hainich()
        sim.Read_config(self.setup.config.config_file)
        sim.Init_input()

        t1 = perf_counter()
        sim.Init_parameters(cparameters)
        sim.Set_water_pot_initials(-1.0, -0.2)

        # Specify Start and End of the Simulation
        format = "%Y-%m-%d %H:%M:%S"
        date_start_str = "2023-06-25 00:00:00"
        date_end_str = "2023-06-29 00:00:00"
        #date_end_str = "2023-08-15 00:00:00"
        timestart = DateTime(date_start_str, format)
        timeend = DateTime(date_end_str, format)

        sim.Init_eval(timestart, timeend)
        # Running SimPHony
        sim.Run(timestart, timeend)
        # Perform the analysis
        sim.Analyse()

        # Getting the raw output data
        output = sim.Get_output()
        # Getting the raw analysis data
        an = sim.Get_analysis()

        # Creating pandas dataframe
        self.df = create_output_df(output, date_start_str)

        t2 = perf_counter()

        print(f"Elapsed {t2 - t1}s.")
app = Flask(__name__)
web_interface = Web_Interface()

@app.route('/')
def index():
    return render_template('index_style.html')

@app.route('/update_plot', methods=['POST'])
def update_plot():

    try:
        k_xylem_sat = float(request.form['slider1'])
        gamma_stem = float(request.form['slider2'])
        psi_leaf_50_close = float(request.form['slider3'])
        gamma_leaf = float(request.form['slider4'])
    except (ValueError, KeyError):
        return jsonify({'error': 'Invalid slider values'}), 400



    # Main model run routine

    web_interface.run_model(k_xylem_sat, gamma_stem, psi_leaf_50_close, gamma_leaf)


    fig1 = go.Figure(data=[
        go.Scatter(x=web_interface.df.index, y=web_interface.df['psiLeaf'], name = 'Leaf'),
        go.Scatter(x=web_interface.df.index, y=web_interface.df['psiStem'], name = 'Stem')
    ])

    fig1.update_layout(
        xaxis=dict(showline=True, linecolor='black', mirror=True, linewidth=1,rangeslider=dict(visible=True)),
        yaxis=dict(showline=True, linecolor='black', mirror=True, linewidth=1),
        xaxis_title="Time",  # X-axis title
        yaxis_title="Water potential" , # Y-axis title
        # yaxis=dict(
        #     range=[-4,0]  # Set y-axis limits here
        # )
        plot_bgcolor='white',

    )

    fig2 = go.Figure(
        data=[go.Scatter(x=web_interface.df.index, y=web_interface.df['gss'])])
    fig2.update_layout(
        xaxis=dict(showline=True, linecolor='black', linewidth=1, mirror=True),
        yaxis=dict(showline=True, linecolor='black', linewidth=1, mirror=True),
        xaxis_title="Time",  # X-axis title
        yaxis_title="Stomatal conductance", # Y-axis title
        plot_bgcolor='white'
    )



    plot_json1 = json.loads(fig1.to_json())  # Parse the JSON string to a Python dict
    plot_json2 = json.loads(fig2.to_json())

    return jsonify({'plot1': plot_json1, 'plot2': plot_json2})  # Send the parsed JSON object

if __name__ == '__main__':
    app.run(debug=True)