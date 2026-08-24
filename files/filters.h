#pragma once

#include<iostream>
#include<vector>
#include<algorithm>
#include<string>
#include<map>

#include "section.h"

bool go2granintsy(int ind_i);
void next_inds(int &ind_i, int &ind_j, int &type);

class table_quant {
public:
    table_quant(Section &section);

    table_quant(int length, int size_byte, int ind_table, std::vector<std::vector<int>> &matrix) : length_(length),
        size_byte_(size_byte), ind_table_(ind_table), matrix_(matrix) {
    };

    table_quant() = default;
    table_quant(const table_quant &) = default;
    table_quant(table_quant &&) = default;
    table_quant &operator=(const table_quant &) = default;
    table_quant &operator=(table_quant &&) = default;

    bool operator==(const table_quant &other) const;

    void create_matrix(Section &section);

    int get_length() const;
    int get_size_byte() const;
    int get_ind_table() const;

    int get_el_matrix(int i, int j) const;

    std::vector<std::vector<int>> get_matrix() const;

private:
    int length_;
    int size_byte_;
    int ind_table_;

    std::vector<std::vector<int>> matrix_;

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
    sof0(int length, int precision, int heigth, int width, int cnt_channels,
        std::vector<channel> channels) : length_(length), precision_(precision), heigth_(heigth), width_(width),
        cnt_channels_(cnt_channels), channels_(channels) {}

    sof0(Section &section);

    bool operator==(const sof0 &other) const;
    static bool sort_channel(const channel &l, const channel &r) {
        return l.id < r.id;
    }

    int get_id_quant(int ind) const;
private:
    int length_;
    int precision_;
    int heigth_;
    int width_;
    int cnt_channels_;
    std::vector<channel> channels_;

    bool flag_use_bytes_;
    bool create_sof0_correct_;
};

struct tree {
    int num;

    tree *l;
    tree *r;
};

class dht {
public:
    dht(int length, int type_dht, int id, bool flag_create_tree, std::map<std::string, int> tree_list) :
        length_(length), type_dht_(type_dht), id_(id), flag_create_tree_(flag_create_tree),
        tree_list_(tree_list) {}

    dht(Section &section);

    bool operator==(const dht &other) const;

    void print_dfs();
    void print_dfs(tree *cur);

    bool dfs(int cur_h, tree *cur, int h, int num, std::string &key);
    void create_tree();

    int get_type_dht() const;
    int get_id() const;

    std::map<std::string, int> get_tree_list() const;

private:
    int length_;
    int type_dht_;
    int id_;
    std::vector<std::vector<int>> codes_;
    tree *start;

    bool flag_create_tree_;
    std::map<std::string, int> tree_list_;

    int sum_bytes_;
    bool flag_use_bytes_;
};

struct channel_sos {
    int id;
    int id_DC;
    int id_AC;

    bool operator==(const channel_sos &other) const;
};

class sos {
public:
    sos(int length, int cnt_channel, std::vector<channel_sos> channels) : length_(length), cnt_channels_(cnt_channel),
                                                                 channels_(channels) {
    }

    sos(Section &section);

    bool operator==(const sos &other) const;

    static bool comp(const channel_sos &l, const channel_sos &r) {
        return l.id < r.id;
    }

    int get_id_dc(int id) const;
    int get_id_ac(int id) const;

private:
    int length_;
    int cnt_channels_;
    std::vector<channel_sos> channels_;

    bool flag_use_bytes_;
};