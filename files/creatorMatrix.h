#pragma once

#include<vector>
#include<map>
#include<string>
#include<iostream>
#include<cmath>

#include "filters.h"

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