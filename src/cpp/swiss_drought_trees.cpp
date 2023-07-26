//
// Created by Phillip on 18.07.23.
//

#include "swiss_drought_trees.h"
#include <set>
#include <filesystem>
#include <string>
#include "csv_reader.h"
namespace fs = std::filesystem;

using std::vector;

Swiss_Drought_Trees::Swiss_Drought_Trees(std::string path) {

    std::set<fs::path> sorted_by_name;

    for (auto &entry : fs::directory_iterator(path))
        sorted_by_name.insert(entry.path());

    for (const auto & entry : sorted_by_name){
        std::string filepath = entry.string();

        CSV_Reader reader(filepath, true, ',');

        std::vector<int> indexes = {2,3};

        vector<vector<double> > data = reader.Get<double>(indexes);

        vector<vector<double> > data_T = transpose(data);

        Tree tree;
        std::vector<int> day_diff_int(data_T[0].begin(), data_T[0].end());
        tree.offsets = day_diff_int;
        tree.psi_leaf = data_T[1];

        if (filepath.find("alive") != std::string::npos) {
            tree.alive = true;
        }
        else{
            tree.alive = false;
        }

        trees.push_back(tree);

    }






}

std::vector<std::vector<double> > Swiss_Drought_Trees::transpose(const std::vector<std::vector<double>> data) {

    std::vector<std::vector<double> > result(data[0].size(),
                                             std::vector<double>(data.size()));
    for (std::vector<double>::size_type i = 0; i < data[0].size(); i++)
        for (std::vector<double>::size_type j = 0; j < data.size(); j++) {
            result[i][j] = data[j][i];
        }
    return result;
}



