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

            if (file->is_open() == false){
                file = std::make_unique<std::fstream>(filename,  std::ios::in);
                if (has_header){
                    getline(*file, line);
                }
            }

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

            if (file->is_open() == false){
                file = std::make_unique<std::fstream>(filename,  std::ios::in);
                if (has_header){
                    getline(*file, line);
                }
            }


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


        template<typename TL>
        TL Convert(string s){
            return nullptr;
        }


    private:
        std::unique_ptr<std::fstream> file;
        char delimiter;
        string filename;
        bool has_header;


    };

    template<>
    inline float CSV_Reader::Convert(std::string s) {
            return std::stof(s);
        };

    template<>
    inline double CSV_Reader::Convert(std::string s) {
        return std::stod(s);
    };

    template<>
    inline std::string CSV_Reader::Convert(std::string s) {
        return s;
    };




};