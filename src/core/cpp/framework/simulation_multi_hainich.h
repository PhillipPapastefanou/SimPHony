//
// Created by Phillip on 16.03.24.
//

#pragma once
#include "../io/output.h"
#include "../io/input_hainich.h"
#include "parameters.h"
#include <memory>
#include "../io/analysis_hainich.h"
#include <tuple>
#include <chrono>
#include "date_time.h"

class Simulation_Multi_Hainich {

public:
    Simulation_Multi_Hainich();
    void Init_input(std::string forcing_file, std::string sap_flow_file, std::string psi_stem_file, int rank);
    void Init_Full_Parameter_Setups(string filename, std::vector<int> ids);
    void Init_Partial_Parameter_Setups(string root_filename, string partial_parameter_filename, std::vector<int> ids);

    void Set_water_pot_initials(double psi_leaf, double psi_stem);
    void Run(DateTime timestart, DateTime timeend);

    std::vector<AnalysisHainich> Get_analysis_list();

    DateTime Get_first_year();
    DateTime Get_last_year();
private:

    int rank;

    std::unique_ptr<Input_Hainich> input;
    std::vector<std::tuple<Parameters, int> > parameter_list;

    std::unique_ptr<TimeSeries> sap_series;
    std::unique_ptr<TimeSeries> psi_stem_series;

    std::vector<AnalysisHainich> analysis_list;

    double init_psi_stem;
    double init_psi_leaf;

    std::string format_duration( std::chrono::milliseconds ms ) {
        using namespace std::chrono;
        auto secs = duration_cast<seconds>(ms);
        ms -= duration_cast<milliseconds>(secs);
        auto mins = duration_cast<minutes>(secs);
        secs -= duration_cast<seconds>(mins);
        auto hour = duration_cast<hours>(mins);
        mins -= duration_cast<minutes>(hour);

        std::stringstream ss;
        std::string hour_str;
        if (hour.count() < 10)
            hour_str = "0";
        hour_str += std::to_string(hour.count());

        std::string min_str;
        if (mins.count() < 10)
            min_str = "0";
        min_str += std::to_string(mins.count());

        std::string sec_str;
        if (secs.count() < 10)
            sec_str = "0";
        sec_str += std::to_string(secs.count());

        ss << hour_str << ":" << min_str << ":" << sec_str;
        return ss.str();
    }

};

