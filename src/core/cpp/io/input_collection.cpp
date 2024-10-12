//
// Created by Phillip on 11.03.24.
//

#include "input_collection.h"
#include <algorithm>
#include <iostream>

InputCollection::InputCollection(std::string filename, bool has_header, char delimiter):
reader(filename, has_header, delimiter){

}

void InputCollection::init_regular(std::string dt_header, std::string format) {

    init_irregular(dt_header, format);

    if(dates.size() < 2){
        std:: cout << "Too few datapoints of input found" << std:: endl;
        exit(99);
    }

    long diff0 = dates[1] - dates[0];
    // Checking for nonregular data
    for (int i = 0; i < dates.size() - 1; ++i) {
        long diff = dates[i+1] - dates[i];
        if (diff != diff0){
            std:: cout << "Nonregular time input found at " << i << std:: endl;
            exit(99);
        }
    }
    time_res_sec = diff0;

}


void InputCollection::init_irregular(std::string dt_header, std::string format) {

    auto index_it = std::find(reader.header.begin(), reader.header.end(), dt_header);

    if (index_it == reader.header.end()){
        std::cout << "Could not find " << dt_header << " header string in header. Available strings:" << std::endl;
        for (auto str: reader.header) {
        std::cout << str << " ";
        }
        std::cout << std::endl;
        std::cout << "Exiting..." << std::endl;
        exit(99);
    }

    int index = std::distance( reader.header.begin(), index_it );
    vector<vector<string>> dates_str = reader.Get<std::string>({index});

    for (int i = 0; i < dates_str.size(); ++i) {

        std::string dstr = dates_str[i][0];
        std::replace( dstr.begin(),
                      dstr.end(),'T', ' ');
        std::replace( dstr.begin(),
                      dstr.end(),'Z', '\0');
        DateTime dt(dstr, format);
        dates.push_back(dt);
    }

}


vector<vector<float> > InputCollection::get_data(vector<int> indexes) {
    return reader.Get<float>(indexes);
}

vector<vector<float> > InputCollection::get_data(vector<string> columns) {
    return reader.Get<float>(columns);
}
