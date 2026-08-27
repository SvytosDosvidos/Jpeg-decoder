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

TableQuant::TableQuant(Section &section) {
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

void TableQuant::create_matrix(Section &section) {
    int ind_i = 0, ind_j = 0;
    std::vector<std::pair<int, int> > directions = {{-1, 1}, {1, -1}};
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

std::vector<std::vector<int> > TableQuant::get_matrix() const {
    return matrix_;
}

bool TableQuant::get_flag_use_bytes() const {
    return flag_use_bytes_;
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

Sof0::Sof0(Section &section) {
    flag_use_bytes_ = true;
    length_ = section.get_length();

    if (length_ < 8) {
        flag_use_bytes_ = false;
        create_sof0_correct_ = false;
        return;
    }

    precision_ = section.get_buffer_el(2);
    height_ = section.get_buffer_el(3, 4);
    width_ = section.get_buffer_el(5, 6);
    cnt_channels_ = section.get_buffer_el(7);
    channels_.resize(cnt_channels_);

    if (3 * cnt_channels_ + 8 != length_) {
        flag_use_bytes_ = false;
        create_sof0_correct_ = false;
        return;
    }

    for (int i = 8; i < section.get_length(); i += 3) {
        int ind = (i - 8) / 3;
        channels_[ind] = {
            section.get_buffer_el(i), section.get_buffer_el(i + 1) / 16,
            section.get_buffer_el(i + 1) % 16, section.get_buffer_el(i + 2)
        };
    }

    sort(channels_.begin(), channels_.end(), sort_channel);

    if (channels_.size() != 3) {
        create_sof0_correct_ = false;
        return;
    }
}

int Sof0::get_id_channel(int ind) const {
    return channels_[ind].id;
}

int Sof0::get_id_quant(int ind) const {
    return channels_[ind].id_quant;
}

bool Sof0::get_flag_use_bytes() const {
    return flag_use_bytes_;
}

//dht
Dht::Dht(Section &section) {
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
}

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

void Dht::create_tree() {
    start = new tree({-1, nullptr, nullptr});
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < codes_[i].size(); j++) {
            std::string key = "";
            flag_create_tree_ &= dfs(0, start, i + 1, codes_[i][j], key);
            tree_list_[key] = codes_[i][j];
            if (!flag_create_tree_) {
                return;
            }
        }
    }
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

bool Dht::get_flag_use_bytes() const {
    return flag_use_bytes_;
}

//sos
Sos::Sos(Section &section) {
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
        return;
        }

    sort(channels_.begin(), channels_.end(), comp);
}

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

bool Sos::get_flag_use_bytes() const {
    return flag_use_bytes_;
}