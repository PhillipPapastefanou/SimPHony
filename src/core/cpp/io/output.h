//
// Created by Phillip on 12.07.23.
//

#pragma once
#include <vector>
#include <string>
#include "../framework/date_time.h"
#include "../framework/parameters.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#endif 

using std::vector;
class Output {

public:
    Output(const Parameters& parameters);
    ~Output();



    void Add_Timestep(double t);
    void Add_DateTime(DateTime t);

    void Add_T(double t);
    void Add_J(double J);
    void Add_G(double G);
    void Add_O(double O);

    void Add_J_adapted_density(double J);

    // scale is applied while appending (float(v * scale) per element) so
    // callers don't need to build a separately-scaled temporary vector --
    // see the Flat_Matrix comment below for why this matters.
    void Add_G_indiv(const vector<double>& G_indiv, double scale = 1.0);

    void Add_psi_leaf(float psi_leaf);
    void Add_psi_sap_ground(float psi_stem_ground);
    void Add_psi_heart_ground(float psi_stem_ground);
    void Add_psi_stems_seg(const vector<double>& psi_stem_seg, double scale = 1.0);
    void Add_psi_soil_indiv(const vector<double>& psi_soil_indiv, double scale = 1.0);

    void Add_gs( double gs);
    void Add_beta(double beta);
    void Add_ks_indiv(const vector<double>& ks_indiv, double scale = 1.0);
    void Add_vpd(double vpd);
    void Add_anet(double anet);

    void Add_steps_psi_leaf(int steps_psi_leaf);
    void Add_steps_psi_stem_ground(int steps_psi_stem);

    void Clear();

    const vector<DateTime> &Get_dates() const;
    const vector<long> &Get_times() const;
    const vector<float> &Get_T() const;
    const vector<float> &Get_J() const;
    const vector<float> &Get_G() const;
    const vector<float> &Get_O() const;

    vector<float> Get_J_per_sap() const;
    vector<float> Get_G_per_sap() const;

    vector<float> Get_J_per_area() const;

    // These reshape from flat storage on demand rather than returning a
    // reference -- see the Flat_Matrix comment below. Cheap relative to a
    // Run() (called at most once per Run(), not once per timestep).
    vector<vector<float> > Get_G_indiv() const;

    const vector<float> &Get_psi_leaf() const;

    const vector<float> &Get_psi_sap() const;

    const vector<float> &Get_psi_heart() const;

    vector<vector<float> > Get_psi_soil_indiv() const;

    const vector<float> &Get_gs() const;

    const vector<float> &Get_beta() const;

    const vector<float> &Get_vpd() const;

    const vector<float> &Get_anet() const;


    vector<vector<float> > Get_ks_soil() const;

    const vector<int> &Get_steps_psi_leaf() const;

    const vector<int> &Get_steps_psi_stem() const;

    void Export_CSV(std::string filename);


    #ifdef __EMSCRIPTEN__
    emscripten::val Get_T_view() const;
    emscripten::val Get_J_view() const;
    emscripten::val Get_psi_leaf_view() const;
    emscripten::val Get_psi_sap_view() const;
    emscripten::val Get_gs_view() const;
    emscripten::val Get_vpd_view() const;
    emscripten::val Get_beta_view() const;
    #endif



private:
    const Parameters& parameters;

    std::vector<long> times;
    std::vector<DateTime> dates;

    vector<float> Ta;
    vector<float> Ja;
    vector<float> Ja_adapted;
    vector<float> Ga;
    vector<float> Oa;

    // Per-layer/per-segment quantities (soil ψ, soil k, per-individual G,
    // per-segment stem ψ) used to be vector<vector<float>>, appended to via
    // push_back(vector<float>) once per timestep. Each push_back allocated a
    // brand-new inner vector -- measured at ~8.4us/step (~24% of total
    // per-step time) for a 2-month interactive run, and it also meant
    // copying an Output (e.g. Simulation_Single::Get_output()'s by-value
    // return) did one heap allocation per timestep per field.
    //
    // Flat_Matrix stores the same data contiguously (row i, column s at
    // data[i*row_width + s]) with ordinary amortized push_back, matching
    // the already-cheap pattern used by e.g. psi_leaf_a below. Reshaping
    // back into vector<vector<float>> happens lazily in the Get_*()
    // accessors, which are called at most once per Run() rather than once
    // per timestep -- and copying an Output now copies a handful of flat
    // buffers instead of thousands of small ones.
    struct Flat_Matrix {
        vector<float> data;
        int row_width = 0;

        void Add_row(const vector<double>& src, double scale) {
            if (row_width == 0) row_width = static_cast<int>(src.size());
            // No explicit reserve() here: calling it every row would ask for
            // exactly data.size()+src.size() each time, capping capacity at
            // exactly what's needed and forcing a reallocation on every
            // single call -- defeating push_back's own amortized (~2x)
            // growth, which already gives O(1) amortized appends.
            for (double v : src) data.push_back(static_cast<float>(v * scale));
        }

        vector<vector<float> > To_nested() const {
            vector<vector<float> > out;
            if (row_width == 0) return out;
            out.reserve(data.size() / row_width);
            for (size_t i = 0; i < data.size(); i += row_width) {
                out.emplace_back(data.begin() + i, data.begin() + i + row_width);
            }
            return out;
        }

        void clear() { data.clear(); row_width = 0; }
    };

    Flat_Matrix Gaa;

    vector<float> psi_leaf_a;
    vector<float> psi_sap_ground_a;
    vector<float> psi_heart_ground_a;
    Flat_Matrix psi_stem_a;
    Flat_Matrix psi_soil_aa;

    vector<float> gs_a;

    vector<float> beta_a;

    vector<float> vpd_a;
    vector<float> anet_a;

    Flat_Matrix k_soil_aa;

    vector<int> steps_psi_leaf_a;
    vector<int> steps_psi_stem_a;




};


