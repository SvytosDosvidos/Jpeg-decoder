#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<map>

#include "section.h"

bool go2granintsy(int ind_i);
void next_inds(int &ind_i, int &ind_j, int &type);

class Marker {
private:

};

class TableQuant : public Marker {
public:
    TableQuant(int length, int size_byte, int ind_table, std::vector<std::vector<int>> &matrix) : length_(length),
        size_byte_(size_byte), ind_table_(ind_table), matrix_(matrix) {}

    TableQuant() = default;

    TableQuant(const TableQuant &) = default;
    TableQuant(TableQuant &&) = default;
    TableQuant &operator=(const TableQuant &) = default;
    TableQuant &operator=(TableQuant &&) = default;

    bool operator==(const TableQuant &other) const;

    static std::vector<std::vector<int>> create_matrix(Section &section) noexcept;

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
};

struct Channel {
    int id;
    int h;
    int w;
    int id_quant;

    bool operator==(const Channel &other) const {
        return id == other.id && h == other.h
               && w == other.w && id_quant == other.id_quant;
    }
};

class Sof0 : public Marker {
public:
    Sof0(int length, int precision, int height, int width, int cnt_channels,
        std::vector<Channel> channels) : length_(length), precision_(precision), height_(height), width_(width),
        cnt_channels_(cnt_channels), channels_(channels) {}

    bool operator==(const Sof0 &other) const;
    static bool sort_channel(const Channel &l, const Channel &r) {
        return l.id < r.id;
    }

    int get_id_quant(int ind) const;
    int get_id_channel(int ind) const;
private:
    int length_;
    int precision_;
    int height_;
    int width_;
    int cnt_channels_;
    std::vector<Channel> channels_;
};

struct tree {
    int num;

    tree *l;
    tree *r;
};

class Dht : public Marker {
public:
    Dht(int length, int type_dht, int id, bool flag_create_tree, std::map<std::string, int> tree_list) :
        length_(length), type_dht_(type_dht), id_(id), flag_create_tree_(flag_create_tree),
        tree_list_(tree_list) {}

    bool operator==(const Dht &other) const;

    static bool dfs(int cur_h, tree *cur, int h, int num, std::string &key);
    static void create_tree(tree* &start, std::vector<std::vector<int>> &codes,
        std::map<std::string, int> &tree_list, bool &flag_create_tree);

    void print_dfs();
    void print_dfs(tree *cur);

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
};

struct channel_sos {
    int id;
    int id_DC;
    int id_AC;

    bool operator==(const channel_sos &other) const;
};

class Sos : public Marker {
public:
    Sos(int length, int cnt_channel, std::vector<channel_sos> channels) : length_(length),
        cnt_channels_(cnt_channel), channels_(channels) {}

    bool operator==(const Sos &other) const;

    static bool sort_sos(const channel_sos &l, const channel_sos &r) {
        return l.id < r.id;
    }

    int get_id_dc(int ind) const;
    int get_id_ac(int ind) const;
    int get_id(int ind) const;
private:
    int length_;
    int cnt_channels_;
    std::vector<channel_sos> channels_;
};