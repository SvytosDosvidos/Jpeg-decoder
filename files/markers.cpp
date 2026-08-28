#include "markers.h"

bool go2granintsy(int ind_i) {
    if (ind_i < 0 || ind_i >= 8) {
        return true;
    }
    return false;
}

void next_inds(int &ind_i, int &ind_j, int &type) {
    std::vector<std::pair<int, int> > directions = {{-1, 1}, {1, -1}};

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

bool channel_sos::operator==(const channel_sos &other) const {
    return id == other.id
    && id_AC == other.id_AC
    && id_DC == other.id_DC;
}

//table_quant
bool TableQuant::operator==(const TableQuant &other) const {
    return length_ == other.length_
           && size_byte_ == other.size_byte_
           && ind_table_ == other.ind_table_
           && matrix_ == other.matrix_;
}

std::vector<std::vector<int>> TableQuant::create_matrix(Section &section) noexcept {
    int ind_i = 0, ind_j = 0;
    std::vector<std::pair<int, int>> directions = {{-1, 1}, {1, -1}};
    int type = 0;
    int ind = 0;
    std::vector<std::vector<int>> matrix(8, std::vector<int>(8));

    while (ind < 64) {
        matrix[ind_i][ind_j] = section.get_buffer_el(3 + ind);

        next_inds(ind_i, ind_j, type);
        ind++;
    }

    return matrix;
}

int TableQuant::get_length() const {
    return length_;
}

int TableQuant::get_size_byte() const {
    return size_byte_;
}

int TableQuant::get_ind_table() const {
    return ind_table_;
}

int TableQuant::get_el_matrix(int i, int j) const {
    return matrix_[i][j];
}

std::vector<std::vector<int>> TableQuant::get_matrix() const {
    return matrix_;
}

//sof0
bool Sof0::operator==(const Sof0 &other) const {
    return length_ == other.length_
           && precision_ == other.precision_
           && height_ == other.height_
           && width_ == other.width_
           && cnt_channels_ == other.cnt_channels_
           && channels_ == other.channels_;
}

int Sof0::get_id_channel(int ind) const {
    return channels_[ind].id;
}

int Sof0::get_id_quant(int ind) const {
    return channels_[ind].id_quant;
}

//dht
bool Dht::operator==(const Dht &other) const {
    return length_ == other.length_
           && type_dht_ == other.type_dht_
           && id_ == other.id_
           && flag_create_tree_ == other.flag_create_tree_
           && tree_list_ == other.tree_list_;
}

bool Dht::dfs(int cur_h, tree *cur, int h, int num, std::string &key) {
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

void Dht::create_tree(tree* &start, std::vector<std::vector<int>> &codes,
    std::map<std::string, int> &tree_list, bool &flag_create_tree) {
    start = new tree({-1, nullptr, nullptr});
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < codes[i].size(); j++) {
            std::string key = "";
            flag_create_tree &= dfs(0, start, i + 1, codes[i][j], key);
            tree_list[key] = codes[i][j];
            if (!flag_create_tree) {
                return;
            }
        }
    }
}

void Dht::print_dfs() {
    std::cout << "start_print\n";
    print_dfs(start);
}

void Dht::print_dfs(tree *cur) {
    if (!cur) {
        std::cout << "\n";
        return;
    }
    std::cout << cur->num << "\n";
    std::cout << "l: ";
    print_dfs(cur->l);
    std::cout << "r: ";
    print_dfs(cur->r);
}

int Dht::get_type_dht() const {
    return type_dht_;
}

int Dht::get_id() const {
    return id_;
}

std::map<std::string, int> Dht::get_tree_list() const {
    return tree_list_;
}

//sos
bool Sos::operator==(const Sos &other) const {
    return length_ == other.length_
           && cnt_channels_ == other.cnt_channels_
           && channels_ == other.channels_;
}

int Sos::get_id_dc(int ind) const {
    return channels_[ind].id_DC;
}

int Sos::get_id_ac(int ind) const {
    return channels_[ind].id_AC;
}

int Sos::get_id(int ind) const {
    return channels_[ind].id;
}