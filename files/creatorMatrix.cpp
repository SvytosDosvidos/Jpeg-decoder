#include "creatorMatrix.h"

creatorMatrix::creatorMatrix() {
    matrix_.resize(8);
    matrix_quant_.resize(8);
    matrix_reverse_cos_.resize(8);
    matrix_final_.resize(8);
    for (int i = 0; i < 8; i++) {
        matrix_[i].resize(8);
        matrix_quant_[i].resize(8);
        matrix_reverse_cos_[i].resize(8);
        matrix_final_[i].resize(8);
    }
}

bool creatorMatrix::operator==(const creatorMatrix &other) const {
    return last_use_byte_ == other.last_use_byte_
           && create_matrix_ == other.create_matrix_
           && find_dc_ == other.find_dc_
           && matrix_ == other.matrix_
           && matrix_quant_ == other.matrix_quant_
           && matrix_reverse_cos_ == other.matrix_reverse_cos_;
}

void creatorMatrix::createMatrix(std::vector<char> &symbols, std::map<std::string, int> tree_list_dc,
                                 std::map<std::string, int> tree_list_ac) {
    find_dc_ = false;
    create_matrix_ = true;
    std::string cur_str;

    int cur_i = 0, cur_j = 0, type = 0;

    for (int i = 0; i < symbols.size(); i++) {
        cur_str.push_back(symbols[i]);
        if (!find_dc_) {
            if (tree_list_dc.find(cur_str) != tree_list_dc.end()) {
                int k_dc = 0;

                if (i + tree_list_dc[cur_str] > symbols.size()) {
                    create_matrix_ = false;
                    last_use_byte_ = i;
                    return;
                }

                for (int j = i + 1; j <= i + tree_list_dc[cur_str]; j++) {
                    k_dc *= 2;
                    k_dc += symbols[j] - '0';
                }

                if (symbols[i + 1] == '0') {
                    k_dc = k_dc - pow(2, tree_list_dc[cur_str]) + 1;
                }

                matrix_[cur_i][cur_j] = k_dc;

                next_inds(cur_i, cur_j, type);

                i += tree_list_dc[cur_str];
                cur_str.clear();

                find_dc_ = true;
            }
        } else {
            if (tree_list_ac.find(cur_str) != tree_list_ac.end()) {
                if (tree_list_ac[cur_str] == 0) {
                    last_use_byte_ = i;
                    return;
                }
                int k_ac_1 = tree_list_ac[cur_str] / 16, k_ac_2 = tree_list_ac[cur_str] % 16;

                while (k_ac_1--) {
                    matrix_[cur_i][cur_j] = 0;
                    if (cur_i == 7 && cur_j == 7) {
                        last_use_byte_ = i;
                        return;
                    }

                    next_inds(cur_i, cur_j, type);
                }

                int k_ac = 0;

                if (i + k_ac_2 > symbols.size()) {
                    last_use_byte_ = i;
                    create_matrix_ = false;
                    return;
                }

                for (int j = i + 1; j <= i + k_ac_2; j++) {
                    k_ac *= 2;
                    k_ac += symbols[j] - '0';
                }

                if (symbols[i + 1] == '0') {
                    k_ac = k_ac - pow(2, k_ac_2) + 1;
                }

                matrix_[cur_i][cur_j] = k_ac;
                if (cur_i == 7 && cur_j == 7) {
                    last_use_byte_ = i;
                    return;
                }

                next_inds(cur_i, cur_j, type);
                i += k_ac_2;
                cur_str.clear();
            }
        }
    }
}

void creatorMatrix::print_matrix() const {
    for (int i = 0; i < matrix_.size(); i++) {
        for (int j = 0; j < matrix_[i].size(); j++) {
            std::cout << matrix_[i][j] << " ";
        }
        std::cout << "\n";
    }

    std::cout << last_use_byte_ << "\n";
}

void creatorMatrix::print_matrix_quant() const {
    for (int i = 0; i < matrix_quant_.size(); i++) {
        for (int j = 0; j < matrix_quant_[i].size(); j++) {
            std::cout << matrix_quant_[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void creatorMatrix::print_matrix_reverse_cos_() const {
    for (int i = 0; i < matrix_reverse_cos_.size(); i++) {
        for (int j = 0; j < matrix_reverse_cos_[i].size(); j++) {
            std::cout << matrix_reverse_cos_[i][j] << " ";
        }
        std::cout << "\n";
    }
}

int creatorMatrix::get_last_use_byte() const {
    return last_use_byte_;
}

int creatorMatrix::get_el_matrix(int ind_i, int ind_j) const {
    return matrix_[ind_i][ind_j];
}

void creatorMatrix::set_el_matrix(int ind_i, int ind_j, int el) {
    matrix_[ind_i][ind_j] = el;
}

void creatorMatrix::set_el_matrix_quant(int ind_i, int ind_j, int el) {
    matrix_quant_[ind_i][ind_j] = el;
}

int creatorMatrix::get_el_matrix_quant(int ind_i, int ind_j) const {
    return matrix_quant_[ind_i][ind_j];
}

void creatorMatrix::set_el_matrix_reverse_cos(int ind_i, int ind_j, int el) {
    matrix_reverse_cos_[ind_i][ind_j] = el;
}

int creatorMatrix::get_el_matrix_reverse_cos(int ind_i, int ind_j) const {
    return matrix_reverse_cos_[ind_i][ind_j];
}

void creatorMatrix::set_el_matrix_final(int ind_i, int ind_j, int el) {
    matrix_final_[ind_i][ind_j] = el;
}

int creatorMatrix::get_el_matrix_final(int ind_i, int ind_j) const {
    return matrix_final_[ind_i][ind_j];
}