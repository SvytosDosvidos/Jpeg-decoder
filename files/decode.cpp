#include<iostream>
#include<vector>
#include<string>
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

    Section() {
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

bool go2granintsy(int ind_i) {
    if (ind_i < 0 || ind_i >= 8) {
        return true;
    }
    return false;
}

void next_inds(int &ind_i, int &ind_j, int &type) {
    vector<pair<int, int> > directions = {{-1, 1}, {1, -1}};

    if (go2granintsy(ind_i + directions[type].first) || go2granintsy(ind_j + directions[type].second)) {
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
        type = 1 - type;
    } else {
        ind_i += directions[type].first;
        ind_j += directions[type].second;
    }
}

const int MARKER = 0xff;

class table_quant {
public:
    table_quant(const table_quant &) = default;

    table_quant(table_quant &&) = default;

    bool operator==(const table_quant &other) const {
        return length_ == other.length_
               && size_byte_ == other.size_byte_
               && ind_table_ == other.ind_table_
               && matrix_ == other.matrix_;
    }

    table_quant &operator=(const table_quant &) = default;

    table_quant &operator=(table_quant &&) = default;

    table_quant(Section &section) {
        flag_use_bytes_ = true;
        length_ = section.get_length();

        if (length_ != 67) {
            flag_use_bytes_ = false;
            return;
        }

        size_byte_ = 1 + section.get_buffer_el(2) / 16;
        ind_table_ = section.get_buffer_el(2) % 16;

        create_matrix(section);
    }

    table_quant(int length, int size_byte, int ind_table, vector<vector<int> > &matrix) : length_(length),
        size_byte_(size_byte), ind_table_(ind_table), matrix_(matrix) {
    };

    void create_matrix(Section &section) {
        int ind_i = 0, ind_j = 0;
        vector<pair<int, int> > directions = {{-1, 1}, {1, -1}};
        int type = 0;
        int ind = 0;
        matrix_.resize(8);
        for (int i = 0; i < 8; i++) {
            matrix_[i].resize(8);
        }

        while (ind < 64) {
            matrix_[ind_i][ind_j] = section.get_buffer_el(3 + ind);

            next_inds(ind_i, ind_j, type);
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

    vector<vector<int> > get_matrix() const {
        return matrix_;
    }

private:
    int length_;
    int size_byte_;
    int ind_table_;

    vector<vector<int> > matrix_;

    bool flag_use_bytes_;
};

struct channel {
    int id;
    int h;
    int w;
    int id_quant;

    bool operator==(const channel &other) const {
        return id == other.id && h == other.h
               && w == other.w && id_quant == other.id_quant;
    }
};

class sof0 {
public:
    bool operator==(const sof0 &other) const {
        return length_ == other.length_
               && precision_ == other.precision_
               && heigth_ == other.heigth_
               && width_ == other.width_
               && cnt_channels_ == other.cnt_channels_
               && channels_ == other.channels_;
    }

    sof0(Section &section) {
        flag_use_bytes_ = true;
        length_ = section.get_length();

        if (length_ < 8) {
            flag_use_bytes_ = false;
            return;
        }

        precision_ = section.get_buffer_el(2);
        heigth_ = section.get_buffer_el(3, 4);
        width_ = section.get_buffer_el(5, 6);
        cnt_channels_ = section.get_buffer_el(7);
        channels_.resize(cnt_channels_);

        if (3 * cnt_channels_ + 8 != length_) {
            flag_use_bytes_ = false;
            return;
        }

        for (int i = 8; i < section.get_length(); i += 3) {
            int ind = (i - 8) / 3;
            channels_[ind] = {
                section.get_buffer_el(i), section.get_buffer_el(i + 1) / 16,
                section.get_buffer_el(i + 1) % 16, section.get_buffer_el(i + 2)
            };
        }
    }

    sof0(int length, int precision, int heigth, int width, int cnt_channels,
         vector<channel> channels) : length_(length), precision_(precision), heigth_(heigth), width_(width),
                                     cnt_channels_(cnt_channels),
                                     channels_(channels) {
    }

private:
    int length_;
    int precision_;
    int heigth_;
    int width_;
    int cnt_channels_;
    vector<channel> channels_;

    bool flag_use_bytes_;
};

struct tree {
    int num;

    tree *l;
    tree *r;
};

class dht {
public:
    dht(int length, int type_dht, int id, bool flag_create_tree,
        map<string, int> tree_list) : length_(length),
                                      type_dht_(type_dht), id_(id), flag_create_tree_(flag_create_tree),
                                      tree_list_(tree_list) {
    }

    dht(Section &section) {
        flag_use_bytes_ = true;
        length_ = section.get_length();
        sum_bytes_ = 0;

        if (length_ < 19) {
            flag_use_bytes_ = false;
            return;
        }

        type_dht_ = section.get_buffer_el(2) / 16;
        id_ = section.get_buffer_el(2) % 16;

        codes_.resize(16);

        for (int i = 3; i < 19; i++) {
            sum_bytes_ += section.get_buffer_el(i);
        }

        if (3 + 16 + sum_bytes_ != length_) {
            flag_use_bytes_ = false;
            return;
        }

        int ind = 19;
        for (int i = 0; i < 16; i++) {
            codes_[i].resize(section.get_buffer_el(i + 3));
            for (int j = 0; j < codes_[i].size(); j++) {
                codes_[i][j] = section.get_buffer_el(j + ind);
            }
            ind += codes_[i].size();
        }

        flag_create_tree_ = true;
        create_tree();

        /*
        cout << "print:\n";
        for (auto &it : tree_list_) {
            cout << "it = " << it.first << " " << it.second << "\n";
        }
        cout << "\n\n";*/
    }

    bool operator==(const dht &other) const {
        return length_ == other.length_
               && type_dht_ == other.type_dht_
               && id_ == other.id_
               && flag_create_tree_ == other.flag_create_tree_
               && tree_list_ == other.tree_list_;
    }

    bool dfs(int cur_h, tree *cur, int h, int num, string &key) {
        if (cur_h < h) {
            if (!cur->l) {
                cur->l = new tree({-1, nullptr, nullptr});
            }

            if (cur->l->num == -1) {
                key += "0";
                bool flag_l = dfs(cur_h + 1, cur->l, h, num, key);
                if (flag_l) {
                    return flag_l;
                }
                key.pop_back();
            }

            if (!cur->r) {
                cur->r = new tree({-1, nullptr, nullptr});
            }

            if (cur->r->num == -1) {
                key += "1";
                bool flag_r = dfs(cur_h + 1, cur->r, h, num, key);
                if (flag_r) {
                    return flag_r;
                }
                key.pop_back();
            }

            return false;
        } else {
            if (cur->num == -1) {
                cur->num = num;
                return true;
            }
            return false;
        }
    }

    void print_dfs() {
        cout << "start_print\n";
        print_dfs(start);
    }

    void print_dfs(tree *cur) {
        if (!cur) {
            cout << "\n";
            return;
        }
        cout << cur->num << "\n";
        cout << "l: ";
        print_dfs(cur->l);
        cout << "r: ";
        print_dfs(cur->r);
    }

    void create_tree() {
        start = new tree({-1, nullptr, nullptr});
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < codes_[i].size(); j++) {
                string key = "";
                flag_create_tree_ &= dfs(0, start, i + 1, codes_[i][j], key);
                tree_list_[key] = codes_[i][j];
                if (!flag_create_tree_) {
                    return;
                }
            }
        }

        cout << "\n";
    }

    int get_type_dht() const {
        return type_dht_;
    }

    int get_id() const {
        return id_;
    }

    map<std::string, int> get_tree_list() const {
        return tree_list_;
    }

private:
    int length_;
    int type_dht_;
    int id_;
    vector<vector<int> > codes_;
    tree *start;

    bool flag_create_tree_;
    map<std::string, int> tree_list_;

    int sum_bytes_;
    bool flag_use_bytes_;
};

struct channel_sos {
    int id;
    int id_DC;
    int id_AC;

    bool operator==(const channel_sos &other) const {
        return id == other.id
               && id_DC == other.id_DC
               && id_AC == other.id_AC;
    }
};

class sos {
public:
    sos(Section &section) {
        flag_use_bytes_ = true;
        length_ = section.get_length();

        if (length_ < 3) {
            flag_use_bytes_ = false;
            return;
        }

        cnt_channels_ = section.get_buffer_el(2);

        if (2 * cnt_channels_ + 3 + 3 != length_) {
            flag_use_bytes_ = false;
            return;
        }

        channels_.resize(cnt_channels_);
        for (int i = 0; i < cnt_channels_; i++) {
            channel_sos cur_channel;
            cur_channel.id = section.get_buffer_el(2 * i + 3);
            cur_channel.id_DC = section.get_buffer_el(2 * i + 4) / 16;
            cur_channel.id_AC = section.get_buffer_el(2 * i + 4) % 16;
            channels_[i] = cur_channel;
        }

        if (section.get_buffer_el(2 * cnt_channels_ + 3) != 0x00 &&
            section.get_buffer_el(2 * cnt_channels_ + 4) != 0x3F &&
            section.get_buffer_el(2 * cnt_channels_ + 5) != 0x00) {
                flag_use_bytes_ = false;
        }

        sort(channels_.begin(), channels_.end(), comp);
    }

    sos(int length, int cnt_channel, vector<channel_sos> channels) : length_(length), cnt_channels_(cnt_channel),
                                                                     channels_(channels) {
    }

    bool operator==(const sos &other) const {
        return length_ == other.length_
               && cnt_channels_ == other.cnt_channels_
               && channels_ == other.channels_;
    }

    static bool comp(const channel_sos& l, const channel_sos& r) {
        return l.id < r.id;
    }

    int get_id_dc(int id) const {
        return channels_[id].id_DC;
    }

    int get_id_ac(int id) const {
        return channels_[id].id_AC;
    }

private:
    int length_;
    int cnt_channels_;
    vector<channel_sos> channels_;

    bool flag_use_bytes_;
};

class creatorMatrix {
public:
    creatorMatrix() {
        matrix_.resize(8);
        for (int i = 0; i < 8; i++) {
            matrix_[i].resize(8);
        }
    }

    void createMatrix(vector<char> &symbols, map<std::string, int> tree_list_dc, map<std::string, int> tree_list_ac) {
        find_dc_ = false;
        create_matrix_ = true;
        string cur_str;

        for (int i = 0; i < 33; i++) {
            cout << symbols[i];
        }
        cout << "\n";

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
                    //cout << "blue = "  << cur_str << " " << tree_list_ac[cur_str] << "\n";
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

                        //cout << "zero = " << cur_i << " " << cur_j << "\n";
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

                    //cout << i + 1 << " " << k_ac << " " << symbols[i + 1] << "\n";
                    if (symbols[i + 1] == '0') {
                        k_ac = k_ac - pow(2, k_ac_2) + 1;
                    }

                    matrix_[cur_i][cur_j] = k_ac;
                    //cout << "inds = " << cur_i << " " << cur_j << " " << k_ac << "\n\n";
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

    void print_matrix() {
        for (int i = 0; i < matrix_.size(); i++) {
            for (int j = 0; j < matrix_[i].size(); j++) {
                cout << matrix_[i][j] << " ";
            }
            cout << "\n";
        }

        cout << last_use_byte_ << "\n";
    }

    int get_last_use_byte() const {
        return last_use_byte_;
    }

private:
    int last_use_byte_;

    bool create_matrix_;
    bool find_dc_;
    vector<vector<int>> matrix_;
};

class decoder {
public:
    void decode(string path) {
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
        map<int, int> mapf;
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

                if (this_section.get_marker() == 0xDA) {
                    findDA = true;
                    sos new_sos(this_section);
                    _soss.push_back(new_sos);
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
                end_symbols_.push_back(buffer[i]);
                i++;
            } else {
                //throw
            }
        }

        createMatrix();
    }

    static bool comp_dht(const dht& l, const dht& r) {
        return l.get_id() < r.get_id();
    }

    void createMatrix() {
        vector<dht> dc;
        vector<dht> ac;

        for (int i = 0; i < get_size_dhts(); i++) {
            if (get_dht(i).get_type_dht() == 0) {
                dc.push_back(get_dht(i));
            } else {
                ac.push_back(get_dht(i));
            }
        }

        sort(dc.begin(), dc.end(), comp_dht);
        sort(ac.begin(), ac.end(), comp_dht);

        vector<char> bytes;
        for (int i = 0; i < end_symbols_.size(); i++) {
            int num = end_symbols_[i];
            string t;
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

            creator_y.print_matrix();
            cout << "\n";
            reverse(bytes.begin(), bytes.end());
            for (int z = 0; z <= creator_y.get_last_use_byte(); z++) {
                bytes.pop_back();
            }
            reverse(bytes.begin(), bytes.end());
        }

        //create matrix Cb
        creatorMatrix creator_Cb;
        int id_dc_cb = _soss[0].get_id_dc(1);
        int id_ac_cb = _soss[0].get_id_ac(1);
        creator_Cb.createMatrix(bytes, dc[id_dc_cb].get_tree_list(), ac[id_ac_cb].get_tree_list());

        creator_Cb.print_matrix();
        cout << "\n";
        reverse(bytes.begin(), bytes.end());
        for (int z = 0; z <= creator_Cb.get_last_use_byte(); z++) {
            bytes.pop_back();
        }
        reverse(bytes.begin(), bytes.end());

        //create matrix Cr
        creatorMatrix creator_Cr;
        int id_dc_cr = _soss[0].get_id_dc(2);
        int id_ac_cr = _soss[0].get_id_ac(2);
        creator_Cr.createMatrix(bytes, dc[id_dc_cr].get_tree_list(), ac[id_ac_cr].get_tree_list());

        creator_Cr.print_matrix();
        cout << "\n";
        reverse(bytes.begin(), bytes.end());
        for (int z = 0; z <= creator_Cr.get_last_use_byte(); z++) {
            bytes.pop_back();
        }
        reverse(bytes.begin(), bytes.end());
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

    int get_size_soss() const {
        return _soss.size();
    }

    bool get_is_open() const {
        return is_open_;
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

    sos get_sos(int ind) const {
        return _soss[ind];
    }

private:
    vector<table_quant> _table_quants;
    vector<sof0> _sof0s;
    vector<dht> _dhts;
    vector<sos> _soss;

    vector<int> end_symbols_;
    bool is_open_;
};