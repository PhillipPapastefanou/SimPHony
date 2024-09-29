//
// Created by Phillip on 11.07.23.
//
#include "csv_reader.h"
#include <sstream>
#include <iostream>
#include <memory>


io::CSV_Reader::CSV_Reader(string filename, bool has_header, char delimiter) {

    this->filename = filename;
    this->delimiter = delimiter;
    this->has_header = has_header;
    vector<vector<string> > data;
    vector<string> row;
    string line, word;

    file = std::make_unique<std::fstream>(filename,  std::ios::in);

    if(file->is_open()){
        if(has_header){
            getline(*file, line);
            std::stringstream str(line);
            while(getline(str, word, delimiter))
                header.push_back(word);
        }

    }
    else{
        std::cout<<"Could not open the file ";
        std::cout<<filename << std::endl;
        exit(99);
    }


}

io::CSV_Reader::~CSV_Reader() {

    if(file->is_open()){
        file->close();
    }

}



