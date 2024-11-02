//
// Created by Phillip on 30/10/2024.
//
#pragma  once
#include<string>
#include<iostream>

enum class Location{
    Swiss_cc,
    Hainich
};

enum class Swiss_soil_water_input_type{
    NLayersMean,
    NLayersMeanOneStd,
    NLayersMeanNStd,
    NLayersIndiv
};


template <typename T>
struct Item {
    Item(std::string key){
        this->key = key;
        this->found= false;
    }

    T Get(){
        return value;
    }

    void Set(T value){
        this-> value = value;
    }

    bool found;
    std::string key;
    T value;
};


class Config {

public:
    Config();
    ~Config();
    void Read(std::string filename);
    void Create_swiss_cc();
    void Create_hainich();
    void Export(std::string filename);

    // Main setting determining which place is to be simulated
    Location location;
    // Forcing path
    Item<std::string> forcing_file;
    // Parameter list path
    Item<std::string> parameters_list_file;

    // -------------------------------
    // Hainich specific configuration
    // -------------------------------
    Item<std::string>  sap_flow_file;
    // Stem water potential input file
    Item<std::string>  psi_stem_file;

    // -------------------------------
    // Swiss_cc specific configuration
    // -------------------------------
    // Soil water path
    Item<std::string> soilwater_file;
    // Tree data folder path
    Item<std::string> swiss_tree_folder_path;
    // Different variations for soil water input exist for the Swiss site
    Item<Swiss_soil_water_input_type> swiss_soil_water_input_type;

private:
    void Parse(Item<std::string> & item, std::string key, std::string value){
        if (key == item.key){
            if (item.found){
                std::cout << "Already parsed " << item.found << std::endl;
                std::cout << "Potential duplicate in list?" << std ::endl;
                std::cout << "Exiting... ";
                exit(99);
            }

            if (value.size() > 0){
                item.value = value;
                item.found = true;
            }
        }
    }

    void Check(const Item<std::string> & item){
        if (!item.found){
            std::cout << "Could not parse " << item.key << std::endl;
            std::cout << "Exiting... ";
            exit(99);
        }
    }
    void Check(const Item<Swiss_soil_water_input_type> & item){
        if (!item.found){
            std::cout << "Could not parse " << item.key << std::endl;
            std::cout << "Exiting... ";
            exit(99);
        }
    }


};

