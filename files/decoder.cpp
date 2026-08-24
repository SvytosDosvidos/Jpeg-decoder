#include "decoder.h"

const int MARKER = 0xff;

void decoder::decode(std::string path) {
    creator_matrix_y_.resize(4);
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        is_open_ = false;
        return;
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    is_open_ = true;

    std::vector<int> buffer(size);
    char byte;
    int ind = 0;
    while (file.get(byte)) {
        buffer[ind] = static_cast<int>(static_cast<unsigned char>(byte));
        ind++;
    }

    std::vector<Section> sections;
    std::map<int, int> mapf;
    mapf[0xD8] = 0;
    mapf[0xFE] = 2; //0xFE, 0xE0 - in local test
    mapf[0xDB] = 2;
    mapf[0xC0] = 2;
    mapf[0xC4] = 2;
    mapf[0xDA] = 2;
    mapf[0xD9] = 0;

    int i = 0;
    bool findDA = false;
    while (i + 1 < size) {
        if (buffer[i] == MARKER && mapf.find(buffer[i + 1]) != mapf.end()) {
            int indStart = i + 2;
            Section this_section(buffer[i + 1], mapf[buffer[i + 1]]);
            for (int j = indStart; j < indStart + this_section.get_cnt_byte_4_length(); j++) {
                this_section.add_length(buffer[j]);
            }

            for (int j = indStart; j < indStart + this_section.get_length(); j++) {
                this_section.add_buffer(buffer[j]);
            }

            i = indStart + this_section.get_length();
            sections.push_back(this_section);

            if (this_section.get_marker() == 0xDB) {
                table_quant new_table_quant(this_section);
                _table_quants.push_back(new_table_quant);
                map_table_quants_[new_table_quant.get_ind_table()] = new_table_quant;
            } else if (this_section.get_marker() == 0xC0) {
                sof0 new_sof0(this_section);
                _sof0s.push_back(new_sof0);
            } else if (this_section.get_marker() == 0xC4) {
                dht new_dht(this_section);
                _dhts.push_back(new_dht);
            } else if (this_section.get_marker() == 0xDA) {
                findDA = true;
                sos new_sos(this_section);
                _soss.push_back(new_sos);
            }
        } else if (findDA) {
            end_symbols_.push_back(buffer[i]);
            i++;
        } else {
            //throw
        }
    }

    createMatrix();
}

void decoder::createMatrix() {
    std::vector<dht> dc;
    std::vector<dht> ac;

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

    //create matrix Y
    for (int i = 0; i < 4; i++) {
        creatorMatrix creator_y;
        int id_dc_y = _soss[0].get_id_dc(0);
        int id_ac_y = _soss[0].get_id_ac(0);
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
    creatorMatrix creator_Cb;
    int id_dc_cb = _soss[0].get_id_dc(1);
    int id_ac_cb = _soss[0].get_id_ac(1);
    creator_Cb.createMatrix(bytes, dc[id_dc_cb].get_tree_list(), ac[id_ac_cb].get_tree_list());

    reverse(bytes.begin(), bytes.end());
    for (int z = 0; z <= creator_Cb.get_last_use_byte(); z++) {
        bytes.pop_back();
    }
    reverse(bytes.begin(), bytes.end());

    creator_matrix_Cb_ = creator_Cb;

    //create matrix Cr
    creatorMatrix creator_Cr;
    int id_dc_cr = _soss[0].get_id_dc(2);
    int id_ac_cr = _soss[0].get_id_ac(2);
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
    //image.print_image();

    image_ = image;
}

void decoder::calculations_quant() {
    for (int i = 0; i < 4; i++) {
        calculations_quant(creator_matrix_y_[i], 0);
    }

    calculations_quant(creator_matrix_Cb_, 1);
    calculations_quant(creator_matrix_Cr_, 2);
}

void decoder::calculations_quant(creatorMatrix &creator, int id_channel) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int id_channel_quant = _sof0s[0].get_id_quant(id_channel);
            int new_el = creator.get_el_matrix(i, j) * map_table_quants_[id_channel_quant].get_el_matrix(i, j);
            creator.set_el_matrix_quant(i, j, new_el);
        }
    }
}

double decoder::find_k_c(int id) const {
    if (id == 0) {
        return 1 / sqrt(2);
    }
    return 1;
}

void decoder::calculations_reverse_cos() {
    for (int i = 0; i < 4; i++) {
        calculations_reverse_cos(creator_matrix_y_[i]);
    }

    calculations_reverse_cos(creator_matrix_Cb_);
    calculations_reverse_cos(creator_matrix_Cr_);
}

void decoder::calculations_reverse_cos(creatorMatrix &creator) {
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

void decoder::calculations_final() {
    for (int i = 0; i < 4; i++) {
        calculations_final(creator_matrix_y_[i]);
    }

    calculations_final(creator_matrix_Cb_);
    calculations_final(creator_matrix_Cr_);
}

void decoder::calculations_final(creatorMatrix &creator) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int new_el = creator.get_el_matrix_reverse_cos(i, j);
            new_el = std::min(std::max(0, new_el + 128), 255);
            creator.set_el_matrix_final(i, j, new_el);
        }
    }
}

int decoder::get_size_table_quants() const {
    return _table_quants.size();
}

int decoder::get_size_sof0s() const {
    return _sof0s.size();
}

int decoder::get_size_dhts() const {
    return _dhts.size();
}

int decoder::get_size_soss() const {
    return _soss.size();
}

bool decoder::get_is_open() const {
    return is_open_;
}

sof0 decoder::get_sof0(int ind) const {
    return _sof0s[ind];
}

dht decoder::get_dht(int ind) const {
    return _dhts[ind];
}

table_quant decoder::get_table_quant(int ind) const {
    return _table_quants[ind];
}

sos decoder::get_sos(int ind) const {
    return _soss[ind];
}

int decoder::get_size_creator_matrix_y() const {
    return creator_matrix_y_.size();
}

creatorMatrix decoder::get_creator_matrix_y(int ind) const {
    return creator_matrix_y_[ind];
}

creatorMatrix decoder::get_creator_matrix_Cb() const {
    return creator_matrix_Cb_;
}

creatorMatrix decoder::get_creator_matrix_Cr() const {
    return creator_matrix_Cr_;
}

Image decoder::get_image() const {
    return image_;
}