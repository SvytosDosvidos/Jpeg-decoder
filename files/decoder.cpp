#include "decoder.h"

const int MARKER = 0xff;

void Decoder::decode(std::string path) {
    creator_matrix_y_.resize(4);
    std::ifstream file(path, std::ios::binary);

    Parser Parser;
    parser_ = parser;

    parser_.decode(path);

    createMatrix();
    image_.print_image();
}

void Decoder::createMatrix() {
    std::vector<Dht> dc;
    std::vector<Dht> ac;

    for (int i = 0; i < get_size_dhts(); i++) {
        if (get_dht(i).get_type_dht() == 0) {
            dc.push_back(get_dht(i));
        } else {
            ac.push_back(get_dht(i));
        }
    }

    std::sort(dc.begin(), dc.end(), comp_dht);
    std::sort(ac.begin(), ac.end(), comp_dht);

    std::vector<char> bytes;
    for (int i = 0; i < end_symbols_.size(); i++) {
        int num = end_symbols_[i];
        std::string t;
        while (num > 0) {
            t.push_back('0' + num % 2);
            num /= 2;
        }

        while (t.size() < 8) {
            t.push_back('0');
        }

        while (!t.empty()) {
            bytes.push_back(t.back());
            t.pop_back();
        }
    }

    int id_Y = -1, id_Cb = -1, id_Cr = -1;
    for (int i = 0; i < 3; i++) {
        if (soss_[0].get_id(i) == sof0s_[0].get_id_channel(0)) {
            id_Y = i;
        } else if (soss_[0].get_id(i) == sof0s_[0].get_id_channel(1)) {
            id_Cb = i;
        } else if (soss_[0].get_id(i) == sof0s_[0].get_id_channel(2)) {
            id_Cr = i;
        }
    }

    if (id_Y < 0 || id_Cb < 0 || id_Cr < 0) {
        throw std::logic_error("Incorrect file structure .jpg");
    }

    //create matrix Y
    for (int i = 0; i < 4; i++) {
        CreatorMatrix creator_y;
        int id_dc_y = soss_[0].get_id_dc(id_Y);
        int id_ac_y = soss_[0].get_id_ac(id_Y);
        creator_y.createMatrix(bytes, dc[id_dc_y].get_tree_list(), ac[id_ac_y].get_tree_list());

        reverse(bytes.begin(), bytes.end());
        for (int z = 0; z <= creator_y.get_last_use_byte(); z++) {
            bytes.pop_back();
        }
        reverse(bytes.begin(), bytes.end());

        if (i != 0) {
            int new_el = creator_matrix_y_[i - 1].get_el_matrix(0, 0) +
                         creator_y.get_el_matrix(0, 0);
            creator_y.set_el_matrix(0, 0, new_el);
        }

        creator_matrix_y_[i] = creator_y;
    }

    //create matrix Cb
    CreatorMatrix creator_Cb;
    int id_dc_cb = soss_[0].get_id_dc(id_Cb);
    int id_ac_cb = soss_[0].get_id_ac(id_Cb);
    creator_Cb.createMatrix(bytes, dc[id_dc_cb].get_tree_list(), ac[id_ac_cb].get_tree_list());

    reverse(bytes.begin(), bytes.end());
    for (int z = 0; z <= creator_Cb.get_last_use_byte(); z++) {
        bytes.pop_back();
    }
    reverse(bytes.begin(), bytes.end());

    creator_matrix_Cb_ = creator_Cb;

    //create matrix Cr
    CreatorMatrix creator_Cr;
    int id_dc_cr = soss_[0].get_id_dc(id_Cr);
    int id_ac_cr = soss_[0].get_id_ac(id_Cr);
    creator_Cr.createMatrix(bytes, dc[id_dc_cr].get_tree_list(), ac[id_ac_cr].get_tree_list());

    reverse(bytes.begin(), bytes.end());
    for (int z = 0; z <= creator_Cr.get_last_use_byte(); z++) {
        bytes.pop_back();
    }
    reverse(bytes.begin(), bytes.end());

    creator_matrix_Cr_ = creator_Cr;

    calculations_quant();
    calculations_reverse_cos();
    calculations_final();

    Image image;
    image.YCbCrToRGB(creator_matrix_y_, creator_matrix_Cb_, creator_matrix_Cr_);

    image_ = image;
}

void Decoder::calculations_quant() {
    for (int i = 0; i < 4; i++) {
        calculations_quant(creator_matrix_y_[i], 0);
    }

    calculations_quant(creator_matrix_Cb_, 1);
    calculations_quant(creator_matrix_Cr_, 2);
}

void Decoder::calculations_quant(CreatorMatrix &creator, int id_channel) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int id_channel_quant = sof0s_[0].get_id_quant(id_channel);
            int new_el = creator.get_el_matrix(i, j) * map_table_quants_[id_channel_quant].get_el_matrix(i, j);
            creator.set_el_matrix_quant(i, j, new_el);
        }
    }
}

double Decoder::find_k_c(int id) const {
    if (id == 0) {
        return 1 / sqrt(2);
    }
    return 1;
}

void Decoder::calculations_reverse_cos() {
    for (int i = 0; i < 4; i++) {
        calculations_reverse_cos(creator_matrix_y_[i]);
    }

    calculations_reverse_cos(creator_matrix_Cb_);
    calculations_reverse_cos(creator_matrix_Cr_);
}

void Decoder::calculations_reverse_cos(CreatorMatrix &creator) {
    //x, u - column, y, v - line
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            double new_el = 0;
            for (int u = 0; u < 8; u++) {
                for (int v = 0; v < 8; v++) {
                    double pi = 3.14159265358979323846;
                    double arg_cos_1 = (2 * x + 1) * u * pi / 16.0;
                    double arg_cos_2 = (2 * y + 1) * v * pi / 16.0;
                    double this_el = 1 / 4.0 * find_k_c(u) * find_k_c(v) * cos(arg_cos_1) * cos(arg_cos_2);
                    this_el *= creator.get_el_matrix_quant(v, u);
                    new_el += this_el;
                }
            }
            creator.set_el_matrix_reverse_cos(y, x, round(new_el));
        }
    }
}

void Decoder::calculations_final() {
    for (int i = 0; i < 4; i++) {
        calculations_final(creator_matrix_y_[i]);
    }

    calculations_final(creator_matrix_Cb_);
    calculations_final(creator_matrix_Cr_);
}

void Decoder::calculations_final(CreatorMatrix &creator) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int new_el = creator.get_el_matrix_reverse_cos(i, j);
            new_el = std::min(std::max(0, new_el + 128), 255);
            creator.set_el_matrix_final(i, j, new_el);
        }
    }
}

int Decoder::get_size_creator_matrix_y() const {
    return creator_matrix_y_.size();
}

CreatorMatrix Decoder::get_creator_matrix_y(int ind) const {
    return creator_matrix_y_[ind];
}

CreatorMatrix Decoder::get_creator_matrix_Cb() const {
    return creator_matrix_Cb_;
}

CreatorMatrix Decoder::get_creator_matrix_Cr() const {
    return creator_matrix_Cr_;
}

Image Decoder::get_image() const {
    return image_;
}