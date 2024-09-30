//
// Created by Phillip on 18.07.23.
//

#include "swiss_drought_trees.h"
#include <set>
#include <filesystem>
#include <string>
#include "../auxil/csv_reader.h"
namespace fs = std::filesystem;

using std::vector;

Swiss_Drought_Trees::Swiss_Drought_Trees(std::string path) {

    std::set<fs::path> sorted_by_name;

    for (auto &entry : fs::directory_iterator(path))
        sorted_by_name.insert(entry.path());

    for (const auto & entry : sorted_by_name){
        std::string filepath = entry.string();

        io::CSV_Reader reader(filepath, true, ',');

        std::vector<int> indexes = {1,2};

        vector<vector<string> > data = reader.Get<string>(indexes);

        Tree tree;

        for (int i = 0; i < data.size(); ++i) {
            tree.psi_leaf.push_back(std::stod(data[i][1]));
        }

        std::string format = "%Y-%m-%d %H:%M:%S";
        for (int i = 0; i < data.size(); ++i) {
            DateTime dt(data[i][0], format);
            tree.dates.push_back(dt);
        }

        if (filepath.find("alive") != std::string::npos) {
            tree.alive = true;
        }
        else{
            tree.alive = false;
        }

        trees.push_back(tree);

    }



}





