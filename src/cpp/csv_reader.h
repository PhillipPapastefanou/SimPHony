//
// Created by Phillip on 11.07.23.
//

#pragma once
#include <vector>
#include <string>
#include <memory>



#include <fstream>
#include <sstream>


using std::string;
using std::vector;

namespace io {
    class CSV_Reader {


    public:
        CSV_Reader(string file, bool has_header, char delimiter = ',');

        ~CSV_Reader();

        vector<string> header;

        template<typename T>
        vector<vector<T> > Get(vector<int> indexes) {

            string line, word;

            vector<vector<T> > data;
            vector<T> row;
            vector<string> row_vec;


            // Read rows
            while (getline(*file, line)) {
                row.clear();
                row_vec.clear();
                std::stringstream row_str(line);

                while (getline(row_str, word, delimiter)) {
                    row_vec.push_back(word);
                }


                for (auto itr: indexes) {
                    row.push_back(Convert<T>(row_vec[itr]));
                }


                //vector<T> slice = vector<T>(row.begin() + col_min, row.begin() + col_max);

                data.push_back(row);
            }

            if (file->is_open()) {
                file->close();
            }

            return data;
        };

        template<typename T>
        vector<vector<T> > Get() {

            string line, word;

            vector<vector<T> > data;
            vector<T> row;
            vector<string> row_vec;


            // Read rows
            while (getline(*file, line)) {
                row.clear();
                row_vec.clear();
                std::stringstream row_str(line);

                while (getline(row_str, word, delimiter)) {
                    row.push_back(word);
                }

                //vector<T> slice = vector<T>(row.begin() + col_min, row.begin() + col_max);

                data.push_back(row);
            }

            if (file->is_open()) {
                file->close();
            }

            return data;
        }


        template<typename T>
        T Convert(string s);

        template<>
        double Convert(string s);

        template<>
        float Convert(string s);

        template<>
        int Convert(string s);

    private:
        std::unique_ptr<std::fstream> file;
        char delimiter;


    };




};