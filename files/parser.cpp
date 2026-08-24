#include "parser.h"

const int MARKER = 0xff;

void parser::decode(std::string path) {
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
    mapf[0xD8] = 0;//start marker
    mapf[0xFE] = 2;//comments marker
    mapf[0xDB] = 2;//table quant
    mapf[0xC0] = 2;//sof0
    mapf[0xC4] = 2;//dht
    mapf[0xDA] = 2;//sos
    mapf[0xD9] = 0;//
    //0xFE, 0xE0 - in local test

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
                table_quants_.push_back(new_table_quant);
                map_table_quants_[new_table_quant.get_ind_table()] = new_table_quant;
            } else if (this_section.get_marker() == 0xC0) {
                sof0 new_sof0(this_section);
                sof0s_.push_back(new_sof0);
            } else if (this_section.get_marker() == 0xC4) {
                dht new_dht(this_section);
                dhts_.push_back(new_dht);
            } else if (this_section.get_marker() == 0xDA) {
                findDA = true;
                sos new_sos(this_section);
                soss_.push_back(new_sos);
            }
        } else if (findDA) {
            end_symbols_.push_back(buffer[i]);
            i++;
        } else {
            throw std::logic_error("Incorrect file structure .jpg");
        }
    }

    corret_parse();
    createMatrix();
    image_.print_image();
}

void parser::corret_parse() {
    corret_parse(table_quants_);
    corret_parse(sof0s_);
    corret_parse(dhts_);
    corret_parse(soss_);
}

template<HasGetFlag T>
void parser::corret_parse(std::vector<T> markers) {
    for (int i = 0; i < markers.size(); i++) {
        if (!markers[i].get_flag_use_bytes()) {
            throw std::logic_error("Incorrect file structure .jpg");
        }
    }
}

void parser::createMatrix() {
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
        creatorMatrix creator_y;
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
    creatorMatrix creator_Cb;
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
    creatorMatrix creator_Cr;
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

void parser::calculations_quant() {
    for (int i = 0; i < 4; i++) {
        calculations_quant(creator_matrix_y_[i], 0);
    }

    calculations_quant(creator_matrix_Cb_, 1);
    calculations_quant(creator_matrix_Cr_, 2);
}

void parser::calculations_quant(creatorMatrix &creator, int id_channel) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int id_channel_quant = sof0s_[0].get_id_quant(id_channel);
            int new_el = creator.get_el_matrix(i, j) * map_table_quants_[id_channel_quant].get_el_matrix(i, j);
            creator.set_el_matrix_quant(i, j, new_el);
        }
    }
}

double parser::find_k_c(int id) const {
    if (id == 0) {
        return 1 / sqrt(2);
    }
    return 1;
}

void parser::calculations_reverse_cos() {
    for (int i = 0; i < 4; i++) {
        calculations_reverse_cos(creator_matrix_y_[i]);
    }

    calculations_reverse_cos(creator_matrix_Cb_);
    calculations_reverse_cos(creator_matrix_Cr_);
}

void parser::calculations_reverse_cos(creatorMatrix &creator) {
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

void parser::calculations_final() {
    for (int i = 0; i < 4; i++) {
        calculations_final(creator_matrix_y_[i]);
    }

    calculations_final(creator_matrix_Cb_);
    calculations_final(creator_matrix_Cr_);
}

void parser::calculations_final(creatorMatrix &creator) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int new_el = creator.get_el_matrix_reverse_cos(i, j);
            new_el = std::min(std::max(0, new_el + 128), 255);
            creator.set_el_matrix_final(i, j, new_el);
        }
    }
}

int parser::get_size_table_quants() const {
    return table_quants_.size();
}

int parser::get_size_sof0s() const {
    return sof0s_.size();
}

int parser::get_size_dhts() const {
    return dhts_.size();
}

int parser::get_size_soss() const {
    return soss_.size();
}

bool parser::get_is_open() const {
    return is_open_;
}

sof0 parser::get_sof0(int ind) const {
    return sof0s_[ind];
}

dht parser::get_dht(int ind) const {
    return dhts_[ind];
}

table_quant parser::get_table_quant(int ind) const {
    return table_quants_[ind];
}

sos parser::get_sos(int ind) const {
    return soss_[ind];
}

int parser::get_size_creator_matrix_y() const {
    return creator_matrix_y_.size();
}

creatorMatrix parser::get_creator_matrix_y(int ind) const {
    return creator_matrix_y_[ind];
}

creatorMatrix parser::get_creator_matrix_Cb() const {
    return creator_matrix_Cb_;
}

creatorMatrix parser::get_creator_matrix_Cr() const {
    return creator_matrix_Cr_;
}

Image parser::get_image() const {
    return image_;
}