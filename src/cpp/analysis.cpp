//
// Created by Phillip on 14.07.23.
//

#include "analysis.h"
#include <algorithm>
#include <memory>
#include "model.h"
#include "swiss_drought_trees.h"

Analysis::Analysis(Leaf_Stem_Implicit_Model* model, const Swiss_Drought_Trees& swiss_drought_trees):
output(model->Get_output()), swiss_drought_trees(swiss_drought_trees) {

    this->swiss_psi_leaf_states = std::make_shared<Tree_Psi_Leaf_State>(swiss_drought_trees);
}

void Analysis::run_peak_analysis() {

    //overall_peak_psi_leaf = find_overall_peak(output.Get_psi_leaf());
    //overall_peak_psi_stem = find_overall_peak(output.Get_psi_stem());


    vector<std::pair<int,int> > ts_of_interest;
    vector<string> slices_names;
    int timestart, timeend;

    // Day 213 is the last
    // Days 200 to 210 is not inclusive

    timestart = 30 * 2 * 24 * 1;
    timeend = 30 * 2 * 24 * 10;
    ts_of_interest.push_back({timestart, timeend});
    slices_names.push_back("beginning");

    timestart = 30 * 2 * 24 * 13;
    timeend = 30 * 2 * 24 * 33;
    ts_of_interest.push_back({timestart, timeend});
    slices_names.push_back("first_drop");


    timestart = 30 * 2 * 24 * 36;
    timeend = 30 * 2 * 24 * 83;
    ts_of_interest.push_back({timestart, timeend});
    slices_names.push_back("second_drop");


    timestart = 30 * 2 * 24 * 84;
    timeend = 30 * 2 * 24 * 146;
    ts_of_interest.push_back({timestart, timeend});
    slices_names.push_back("third_drop");

    timestart = 30 * 2 * 24 * 36;
    timeend = 30 * 2 * 24 * 146;
    ts_of_interest.push_back({timestart, timeend});
    slices_names.push_back("complete_drop");


    timestart = 30 * 2 * 24 * 200;
    timeend = 30 * 2 * 24 * 210;
    ts_of_interest.push_back({timestart, timeend});
    slices_names.push_back("recovered");



    int running_index = 0;
    for (auto s : slices_names) {

        int timestart_local = ts_of_interest[running_index].first;
        int timeend_local = ts_of_interest[running_index].second;

        TimeSlice slice;
        slice.Init("psi_leaf_" + s, output.Get_psi_leaf(), timestart_local,  timeend_local);
        slices.push_back(slice);

        slice = TimeSlice();
        slice.Init("psi_stem_" + s, output.Get_psi_stem(), timestart_local,  timeend_local);
        slices.push_back(slice);

        slice = TimeSlice();
        slice.Init("beta_" + s, output.Get_beta(), timestart_local,  timeend_local);
        slices.push_back(slice);

        running_index += 1;
    }





}

float Analysis::find_overall_peak(const std::vector<float> &values) {

    return *std::min_element(values.begin(), values.end());
}

void Analysis::Run() {

    run_peak_analysis();

    compare_psi_model_obs();

}

void Analysis::compare_psi_model_obs() {

    this->swiss_psi_leaf_states->Calculate_rmse(output.Get_psi_stem());

}

std::vector<TimeSlice> Analysis::Get_time_slices() {
    return slices;
}

std::vector<double> Analysis::Get_rmse() {
    return swiss_psi_leaf_states->Get_rmse_data();
}


void TimeSlice::CalculatePeaks() {

    minimum = 1E99;
    maximum = -1E99;
    for (int i = 0; i < slice.size(); ++i) {

        double value = slice[i];
        if(value < minimum)
            minimum = value;
        if(value > maximum)
            maximum = value;
    }
    // Todo Figure out why minmax and the second result is not working
//   auto result =   std::minmax(slice.begin(), slice.end());
//   minimum =  *result.first;
//   maximum =  *result.second;

}

TimeSlice::TimeSlice() {

}

void TimeSlice::Init(std::string name, const vector<float> &values, double ts_min, double ts_max) {
    const double steplen = 30.0;

    this->name = name;

    i_min = static_cast<int>(ts_min/steplen);
    i_max = static_cast<int>(ts_max/steplen);

    // Todo Can this done in one step?
    vector<float> slice_s(values.begin() + i_min, values.begin() + i_max);
    slice = slice_s;

    CalculatePeaks();
}



Tree_Psi_Leaf_State::Tree_Psi_Leaf_State(const Swiss_Drought_Trees& swiss_drought_trees):
swiss_drought_trees(swiss_drought_trees){

}

void Tree_Psi_Leaf_State::Calculate_rmse(const vector<float> &values) {


    for (int t = 0; t < swiss_drought_trees.trees.size(); ++t) {

        const Tree& tree = swiss_drought_trees.trees[t];

        std::vector<double> diffs;

        for (int s = 0; s < tree.offsets.size(); ++s) {

            int offset = tree.offsets[s];

            // look for the most negative water potential per day
            int timestart =  2 * 24 * offset;
            int timeend = 2 * 24 * (offset + 1);

            // Allow for some temporal variation
            // the maximum of the model can also be some days earlier or later
            const int number_of_days_off_from_obs = 1;

            // Get the water potential of this day
            std::vector<float> day_slice(values.begin() + timestart - number_of_days_off_from_obs,
                                         values.begin() + timeend + number_of_days_off_from_obs);

            double psi_model  = *std::min_element(day_slice.begin(), day_slice.end());

            // Find the minimum value of the dataset
            double psi_observed = tree.psi_leaf[s];

            diffs.push_back(psi_model - psi_observed);
        }



        double rmse = 0.0;

        for (double d : diffs){
            rmse += (d * d);
        }

        rmse /= diffs.size();
        rmse = std::sqrt(rmse);
        rmse_data.push_back(rmse);

    }
}

std::vector<double> Tree_Psi_Leaf_State::Get_rmse_data() {
    return rmse_data;
}
