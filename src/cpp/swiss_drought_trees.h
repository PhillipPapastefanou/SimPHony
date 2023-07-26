//
// Created by Phillip on 18.07.23.
//

#pragma once
#include <string>
#include <vector>


class Tree;

class Swiss_Drought_Trees {

public:
    Swiss_Drought_Trees(std::string path);
    std::vector<Tree> trees;


private:
    std::vector<std::vector<double> > transpose(const std::vector<std::vector<double> > data);

};

class Tree{
public:
    bool alive;
    std::vector<int> offsets;
    std::vector<double> psi_leaf;




};


