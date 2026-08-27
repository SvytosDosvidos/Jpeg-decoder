#pragma once

#include <vector>
#include "creator_matrix.h"

class Image {
public:
    struct pixel {
        int R;
        int G;
        int B;
    };

    Image();

    pixel YCbCrToRGB_nums(double Y, double Cb, double Cr);
    void YCbCrToRGB(std::vector<CreatorMatrix> &Y, CreatorMatrix &Cb, CreatorMatrix &Cr);

    void print_image() const;

    int get_y_num(std::vector<CreatorMatrix> Y, int ind_i, int ind_j);

    int get_el_matrix_r(int ind_i, int ind_j) const;
    int get_el_matrix_g(int ind_i, int ind_j) const;
    int get_el_matrix_b(int ind_i, int ind_j) const;
private:
    std::vector<std::vector<pixel>> RGB_;
};