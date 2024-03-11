//
// Created by Phillip on 11.03.24.
//

#include "forcing_input_collection.h"
#include <algorithm>
#include <iostream>

ForcingInputCollection::ForcingInputCollection(std::string filename, bool has_header, char delimiter):
reader(filename, has_header, delimiter){

}

void ForcingInputCollection::init(std::string dt_header, std::string format) {

    auto index_it = std::find(reader.header.begin(), reader.header.end(), dt_header);

    if (index_it == reader.header.end()){
       std::cout << "Could not find " << dt_header << " header string." << std::endl;
       std::cout << "Exiting..." << std::endl;
       exit(99);
     }

    int index = std::distance( reader.header.begin(), index_it );

    vector<vector<string>> dates_str = reader.Get<std::string>({index});

    for (int i = 0; i < dates_str.size(); ++i) {

        DateTime dt(dates_str[i][0], format);
        dates.push_back(dt);
    }


    // Checking for nonregular data
    for (int i = 0; i < dates.size() - 1; ++i) {
        long diff = dates[i+1] - dates[i];
        if (diff != 1800){
            std:: cout << "Nonregular input found at " << i << std:: endl;
            exit(99);
        }
    }

}

vector<vector<float> > ForcingInputCollection::get_data(vector<int> indexes) {
    return reader.Get<float>(indexes);
}
