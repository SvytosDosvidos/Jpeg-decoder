#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<fstream>

#include "creatorMatrix.h"
#include "filters.h"
#include "image.h"

#include "../cmake-build-debug/_deps/catch2-src/src/catch2/generators/catch_generators.hpp"

class decoder {
public:
    void decode(std::string path);

    static bool comp_dht(const dht& l, const dht& r) {
        return l.get_id() < r.get_id();
    }

    void createMatrix();

    void calculations_quant();
    void calculations_quant(creatorMatrix &creator, int id_channel);

    double find_k_c(int id) const;
    void calculations_reverse_cos();
    void calculations_reverse_cos(creatorMatrix &creator);

    void calculations_final();

    void calculations_final(creatorMatrix &creator);

    int get_size_table_quants() const;
    int get_size_sof0s() const;
    int get_size_dhts() const;
    int get_size_soss() const;

    bool get_is_open() const;

    sof0 get_sof0(int ind) const;
    dht get_dht(int ind) const;
    table_quant get_table_quant(int ind) const;

    sos get_sos(int ind) const;

    int get_size_creator_matrix_y() const;

    creatorMatrix get_creator_matrix_y(int ind) const;
    creatorMatrix get_creator_matrix_Cb() const;
    creatorMatrix get_creator_matrix_Cr() const;

    Image get_image() const;
private:
    std::vector<table_quant> _table_quants;
    std::vector<sof0> _sof0s;
    std::vector<dht> _dhts;
    std::vector<sos> _soss;

    std::map<int, table_quant> map_table_quants_;

    std::vector<int> end_symbols_;
    bool is_open_;

    std::vector<creatorMatrix> creator_matrix_y_;
    creatorMatrix creator_matrix_Cb_;
    creatorMatrix creator_matrix_Cr_;

    Image image_;
};