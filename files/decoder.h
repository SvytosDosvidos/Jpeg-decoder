#pragma once
#include<vector>
#include<string>

#include "parser.h"
#include "creator_matrix.h"
#include "image.h"

#include "../cmake-build-debug/_deps/catch2-src/src/catch2/generators/catch_generators.hpp"

class Decoder {
public:
    Decoder() {};

    void decode(std::string path);

    void createMatrix();

    void calculations_quant();
    void calculations_quant(CreatorMatrix &creator, int id_channel);

    double find_k_c(int id) const;
    void calculations_reverse_cos();
    void calculations_reverse_cos(CreatorMatrix &creator);

    void calculations_final();

    void calculations_final(CreatorMatrix &creator);

    int get_size_creator_matrix_y() const;

    CreatorMatrix get_creator_matrix_y(int ind) const;
    CreatorMatrix get_creator_matrix_Cb() const;
    CreatorMatrix get_creator_matrix_Cr() const;

    Parser get_parser() const;
    Image get_image() const;
private:
    Parser parser_;

    std::vector<CreatorMatrix> creator_matrix_y_;
    CreatorMatrix creator_matrix_Cb_;
    CreatorMatrix creator_matrix_Cr_;

    Image image_;
};