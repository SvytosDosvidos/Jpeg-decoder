#include "image.h"

Image::Image() {
    RGB_.resize(16);
    for (int i = 0; i < 16; i++) {
        RGB_[i].resize(16);
    }
};

Image::pixel Image::YCbCrToRGB_nums(double Y, double Cb, double Cr) {
    pixel pix;

    pix.R = round(Y + 1.402 * (Cr - 128));
    pix.G = round(Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128));
    pix.B = round(Y + 1.772 * (Cb - 128));

    pix.R = std::min(std::max(0, pix.R), 255);
    pix.G = std::min(std::max(0, pix.G), 255);
    pix.B = std::min(std::max(0, pix.B), 255);

    return pix;
}

void Image::YCbCrToRGB(std::vector<CreatorMatrix> &Y, CreatorMatrix &Cb, CreatorMatrix &Cr) {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            int Y_num = get_y_num(Y, y, x);
            int Cb_num = Cb.get_el_matrix_final(y / 2, x / 2);
            int Cr_num = Cr.get_el_matrix_final(y / 2, x / 2);

            RGB_[y][x] = YCbCrToRGB_nums(Y_num, Cb_num, Cr_num);
        }
    }
}

void Image::print_image() const {
    std::cout << "R:\n";
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            std::cout << RGB_[i][j].R << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout << "G:\n";
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            std::cout << RGB_[i][j].G << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout << "B:\n";
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            std::cout << RGB_[i][j].B << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int Image::get_y_num(std::vector<CreatorMatrix> Y, int ind_i, int ind_j) {
    if (ind_i <= 7 && ind_j <= 7) {
        return Y[0].get_el_matrix_final(ind_i, ind_j);
    } else if (ind_i <= 7 && ind_j >= 8) {
        return Y[1].get_el_matrix_final(ind_i, ind_j - 8);
    } else if (ind_i >= 8 && ind_j <= 7) {
        return Y[2].get_el_matrix_final(ind_i - 8, ind_j);
    } else if (ind_i >= 8 && ind_j >= 8) {
        return Y[3].get_el_matrix_final(ind_i - 8, ind_j - 8);
    }
}

int Image::get_el_matrix_r(int ind_i, int ind_j) const {
    return RGB_[ind_i][ind_j].R;
}

int Image::get_el_matrix_g(int ind_i, int ind_j) const {
    return RGB_[ind_i][ind_j].G;
}

int Image::get_el_matrix_b(int ind_i, int ind_j) const {
    return RGB_[ind_i][ind_j].B;
}