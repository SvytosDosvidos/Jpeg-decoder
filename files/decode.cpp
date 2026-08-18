#include<iostream>
#include<vector>
#include<string>
#include<set>
#include<map>
#include<math.h>
#include<algorithm>
#include<fstream>
#include<unordered_map>

#include "../cmake-build-debug/_deps/catch2-src/src/catch2/generators/catch_generators.hpp"

using namespace std;

class Section {
public:
    Section(int marker, int get_cnt_byte_4_length) : marker_(marker),
                                                     get_cnt_byte_4_length_(get_cnt_byte_4_length), length_(0),
                                                     buffer_({}) {
    }

    void add_buffer(int num) {
        if (buffer_.capacity() != length_) {
            buffer_.reserve(length_);
        }
        buffer_.push_back(num);
    }

    int get_marker() const {
        return marker_;
    }

    int get_length() const {
        return length_;
    }

    int get_cnt_byte_4_length() const {
        return get_cnt_byte_4_length_;
    }

    int get_buffer_el(int l, int r) const {
        long long el = 0;
        for (int ind = l; ind <= r; ind++) {
            el = 16 * el + buffer_[ind];
        }
        return el;
    }

    int get_buffer_el(int ind) const {
        return buffer_[ind];
    }

    void add_length(int length) {
        length_ = 16 * length_ + length;
    }

private:
    int marker_;
    int length_;
    std::vector<int> buffer_;

    int get_cnt_byte_4_length_;
};

const int MARKER = 0xff;

class table_quant {
public:
    table_quant(const table_quant &) = default;

    table_quant(table_quant &&) = default;

    bool operator==(const table_quant& other) const {
        return length_ == other.length_
        && size_byte_ == other.size_byte_
        && ind_table_ == other.ind_table_
        && matrix_ == other.matrix_;
    }

    table_quant &operator=(const table_quant &) = default;

    table_quant &operator=(table_quant &&) = default;

    table_quant(Section &section) {
        length_ = section.get_length();
        size_byte_ = 1 + section.get_buffer_el(2) / 16;
        ind_table_ = section.get_buffer_el(2) % 16;

        create_matrix(section);
    }

    table_quant(int length, int size_byte, int ind_table, vector<vector<int>> &matrix) :
        length_(length), size_byte_(size_byte), ind_table_(ind_table), matrix_(matrix) {};

    bool go2granintsy(int ind_i) {
        if (ind_i < 0 || ind_i >= 8) {
            return true;
        }
        return false;
    }

    void create_matrix(Section &section) {
        int ind_i = 0, ind_j = 0;
        vector<pair<int, int>> directions = {{-1,1}, {1, -1}};
        int type = 0;
        int ind = 0;
        matrix_.resize(8);
        for (int i = 0; i < 8; i++) {
            matrix_[i].resize(8);
        }

        while (ind < 64) {
            matrix_[ind_i][ind_j] = section.get_buffer_el(3 + ind);
            if (go2granintsy(ind_i + directions[type].first) || go2granintsy(ind_j + directions[type].second)) {
                ind++;
                if (type == 0) {
                    if (!go2granintsy(ind_j + 1)) {
                        ind_j++;
                    } else {
                        ind_i++;
                    }
                } else {
                    if (!go2granintsy(ind_i + 1)) {
                        ind_i++;
                    } else {
                        ind_j++;
                    }
                }
                matrix_[ind_i][ind_j] = section.get_buffer_el(3 + ind);
                type = 1 - type;
            }
            ind_i += directions[type].first;
            ind_j += directions[type].second;
            ind++;
        }
    }

    int get_length() const {
        return length_;
    }

    int get_size_byte() const {
        return size_byte_;
    }

    int get_ind_table() const {
        return ind_table_;
    }

    int get_el_matrix(int i, int j) const {
        return matrix_[i][j];
    }

    vector<vector<int>> get_matrix() const {
        return matrix_;
    }

private:
    int length_;
    int size_byte_;
    int ind_table_;

    vector<vector<int>> matrix_;
};

struct channel {
    int id;
    int h;
    int w;
    int id_quant;

    bool operator==(const channel& other) const {
        return id == other.id && h == other.h
        && w == other.w && id_quant == other.id_quant;
    }
};

class sof0 {
public:
    bool operator==(const sof0& other) const {
        return length_ == other.length_
        && precision_ == other.precision_
        && heigth_ == other.heigth_
        && width_ == other.width_
        && cnt_channels_ == other.cnt_channels_
        && channels_ == other.channels_;
    }

    sof0(Section &section) {
        length_ = section.get_length();
        precision_ = section.get_buffer_el(2);
        heigth_ = section.get_buffer_el(3, 4);
        width_ = section.get_buffer_el(5, 6);
        cnt_channels_ = section.get_buffer_el(7);
        channels_.resize(cnt_channels_);
        for (int i = 8; i < section.get_length(); i += 3) {
            int ind = (i - 8) / 3;
            channels_[ind] = {
                section.get_buffer_el(i), section.get_buffer_el(i + 1) / 16,
                section.get_buffer_el(i + 1) % 16, section.get_buffer_el(i + 2)
            };
        }
    }

    sof0(int length, int precision, int heigth, int width, int cnt_channels, vector<channel> channels) :
    length_(length), precision_(precision), heigth_(heigth), width_(width), cnt_channels_(cnt_channels),
    channels_(channels) {}

private:
    int length_;
    int precision_;
    int heigth_;
    int width_;
    int cnt_channels_;
    vector<channel> channels_;
};

class dht {
private:
    struct tree {
        int num;

        tree *l;
        tree *r;
    };

public:
    dht(Section &section) {
        length_ = section.get_length();
        class_ = section.get_buffer_el(2) / 16;
        id_ = section.get_buffer_el(2) % 16;

        codes_.resize(16);

        int ind = 19;
        for (int i = 0; i < 16; i++) {
            codes_[i].resize(section.get_buffer_el(i + 3));
            for (int j = 0; j < codes_[i].size(); j++) {
                codes_[i][j] = section.get_buffer_el(j + ind);
            }
            ind += codes_[i].size();
        }
    }

    void dfs(tree *v, int num, int this_d, int d) {
        if (!v->l || (v->l && v->l->num == -1)) {
            if (!v->l) {
                *v->l = {-1, nullptr, nullptr};
                if (this_d == d) {
                    v->l->num = num;
                    return;
                } else {
                    dfs(v->l, num, this_d + 1, d);
                }
            } else {
                dfs(v->l, num, this_d + 1, d);
            }
        }

        if (!v->l || (v->l && v->l->num == -1)) {
            if (!v->l) {
                *v->l = {-1, nullptr, nullptr};
                if (this_d == d) {
                    v->l->num = num;
                    return;
                } else {
                    dfs(v->l, num, this_d + 1, d);
                }
            } else {
                dfs(v->l, num, this_d + 1, d);
            }
        }
    }

    void create_tree() {
        *start = {-1, nullptr, nullptr};
        for (int i = 0; i < codes_.size(); i++) {
            int cnt = codes_[i].size();
            while (cnt--) {
            }
        }
    }

private:
    int length_;
    int class_;
    int id_;
    vector<vector<int>> codes_;
    tree *start;
};

class decoder {
public:
    void decode(string path) {
        std::ifstream file(path, std::ios::binary);

        if (!file.is_open()) {
            _is_open = false;
            return;
        }

        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        _is_open = true;

        std::vector<int> buffer(size);
        char byte;
        int ind = 0;
        while (file.get(byte)) {
            buffer[ind] = static_cast<int>(static_cast<unsigned char>(byte));
            ind++;
        }

        std::vector<Section> sections;
        map<int, int> mapf;
        mapf[0xD8] = 0;
        mapf[0xFE] = 2; //0xFE, 0xE0 - in local test
        mapf[0xDB] = 2;
        mapf[0xC0] = 2;
        mapf[0xC4] = 2;
        mapf[0xDA] = 2;
        mapf[0xD9] = 0;

        int i = 0;
        vector<int> nums;
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

                if (this_section.get_marker() == 0xDA) {
                    findDA = true;
                } else if (this_section.get_marker() == 0xDB) {
                    table_quant new_table_quant(this_section);
                    _table_quants.push_back(new_table_quant);
                } else if (this_section.get_marker() == 0xC0) {
                    sof0 new_sof0(this_section);
                    _sof0s.push_back(new_sof0);
                } else if (this_section.get_marker() == 0xC4) {
                    dht new_dht(this_section);
                    _dhts.push_back(new_dht);
                }
            } else if (findDA) {
                nums.push_back(buffer[i]);
                i++;
            } else {
                //throw
            }
        }
    }

    int get_size_table_quants() const {
        return _table_quants.size();
    }

    int get_size_sof0s() const {
        return _sof0s.size();
    }

    int get_size_dhts() const {
        return _dhts.size();
    }

    bool get_is_open() const {
        return _is_open;
    }

    sof0 get_sof0(int ind) const {
        return _sof0s[ind];
    }

    dht get_dht(int ind) const {
        return _dhts[ind];
    }

    table_quant get_table_quant(int ind) const {
        return _table_quants[ind];
    }
private:
    vector<table_quant> _table_quants;
    vector<sof0> _sof0s;
    vector<dht> _dhts;

    bool _is_open;
};