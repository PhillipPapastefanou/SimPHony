//
// Created by Phillip on 18.07.23.
//

#pragma once
#include <string>
#include <vector>
#include "../framework/date_time.h"


class Tree;

class Swiss_Drought_Trees {

public:
    Swiss_Drought_Trees(std::string path);
    std::vector<Tree> trees;
};

class Tree{
public:
    bool alive;
    std::vector<DateTime> dates;
    std::vector<double> psi_leaf;


private:
    template<typename T>
    std::vector<std::vector<T> > transpose(const std::vector<std::vector<T>> data) {

        std::vector<std::vector<T> > result(data[0].size(),
                                                 std::vector<T>(data.size()));
        for (size_t i = 0; i < data[0].size(); i++)
            for (size_t j = 0; j < data.size(); j++) {
                result[i][j] = data[j][i];
            }
        return result;
    }
};


