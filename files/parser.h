#pragma once

#include<vector>
#include<string>
#include<map>
#include<fstream>

#include "creator_marker.h"
#include "markers.h"

#include "../cmake-build-debug/_deps/catch2-src/src/catch2/generators/catch_generators.hpp"

template <typename T>
concept HasGetFlag = requires(T t) {
    t.get_flag_use_bytes();
};

class Parser {
public:
    Parser() {}

    void intiCreator();
    void parse(std::string path);

    void ProcessingEndSymbols(std::vector<char> &bits);

    int get_size_table_quants() const;
    int get_size_sof0s() const;
    int get_size_dhts() const;
    int get_size_soss() const;

    bool get_is_open() const;

    static bool comp_dht(const Dht& l, const Dht& r) {
        return l.get_id() < r.get_id();
    }

    Sof0 get_sof0(int ind) const;
    Dht get_dht(int ind) const;
    TableQuant get_table_quant(int ind) const;
    TableQuant get_map_table_quant(int ind);
    Sos get_sos(int ind) const;

    int get_size_end_symbols() const;
    int get_el_end_symbol(int ind) const;
private:
    CreatorMarker creator_marker_;

    std::vector<TableQuant> table_quants_;
    std::vector<Sof0> sof0s_;
    std::vector<Dht> dhts_;
    std::vector<Sos> soss_;

    std::map<int, TableQuant> map_table_quants_;

    std::vector<int> end_symbols_;
    bool is_open_;
};