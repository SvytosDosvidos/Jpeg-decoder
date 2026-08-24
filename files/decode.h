#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<math.h>
#include<algorithm>
#include<fstream>
#include<cmath>

#include "section.h"
#include "filters.h"
#include "../cmake-build-debug/_deps/catch2-src/src/catch2/generators/catch_generators.hpp"


class creatorMatrix {
public:
    creatorMatrix(int last_use_byte, bool create_matrix, bool find_dc, std::vector<std::vector<int>> matrix,
        std::vector<std::vector<int>> matrix_quant, std::vector<std::vector<int>> matrix_reverse_cos) : last_use_byte_(last_use_byte),
        create_matrix_(create_matrix), find_dc_(find_dc), matrix_(matrix), matrix_quant_(matrix_quant),
        matrix_reverse_cos_(matrix_reverse_cos) {}

    creatorMatrix();

    bool operator== (const creatorMatrix &other) const;

    void createMatrix(std::vector<char> &symbols, std::map<std::string, int> tree_list_dc,
        std::map<std::string, int> tree_list_ac);

    void print_matrix() const;
    void print_matrix_quant() const;
    void print_matrix_reverse_cos_() const;

    int get_last_use_byte() const;

    int get_el_matrix(int ind_i, int ind_j) const;
    void set_el_matrix(int ind_i, int ind_j, int el);

    int get_el_matrix_quant(int ind_i, int ind_j) const;
    void set_el_matrix_quant(int ind_i, int ind_j, int el);

    int get_el_matrix_reverse_cos(int ind_i, int ind_j) const;
    void set_el_matrix_reverse_cos(int ind_i, int ind_j, int el);

    int get_el_matrix_final(int ind_i, int ind_j) const;
    void set_el_matrix_final(int ind_i, int ind_j, int el);

private:
    int last_use_byte_;

    bool create_matrix_;
    bool find_dc_;
    std::vector<std::vector<int>> matrix_;
    std::vector<std::vector<int>> matrix_quant_;
    std::vector<std::vector<int>> matrix_reverse_cos_;
    std::vector<std::vector<int>> matrix_final_;
};

class Image {
public:
    struct pixel {
        int R;
        int G;
        int B;
    };

    Image();

    pixel YCbCrToRGB(double Y, double Cb, double Cr);
    void YCbCrToRGB(std::vector<creatorMatrix> &Y, creatorMatrix &Cb, creatorMatrix &Cr);

    void print_image() const;

    int get_y_num(std::vector<creatorMatrix> Y, int ind_i, int ind_j);

    int get_el_matrix_r(int ind_i, int ind_j) const;
    int get_el_matrix_g(int ind_i, int ind_j) const;
    int get_el_matrix_b(int ind_i, int ind_j) const;
private:
    std::vector<std::vector<pixel>> RGB_;
};

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