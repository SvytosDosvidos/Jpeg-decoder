#include <catch2/catch_test_macros.hpp>
#include <filesystem>

#include "decode.cpp" // временно, пока нет decode.hpp

TEST_CASE("First test") {
    CHECK(2 + 2 == 4);
}

TEST_CASE("test_jpg.jpg") {
    decoder jpg_decoder;

    const std::filesystem::path path =
        std::filesystem::path(TEST_DATA_DIR) / "test_jpg.jpg";

    REQUIRE(std::filesystem::exists(path));

    REQUIRE_NOTHROW(
        jpg_decoder.decode(path.string())
    );

    CHECK(jpg_decoder.get_is_open());

    CHECK(jpg_decoder.get_size_table_quants() == 2);
    CHECK(jpg_decoder.get_size_sof0s() == 1);
    CHECK(jpg_decoder.get_size_dhts() == 4);
    CHECK(jpg_decoder.get_size_soss() == 1);

    //check table_quants
    vector<table_quant> table_quants;

    //first table_quant
    vector<vector<int>> first_matrix = {
        {0xA0, 0x6E, 0x64, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF},
        {0x78, 0x78, 0x8C, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF},
        {0x8C, 0x82, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF},
        {0x8C, 0xAA, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xB4, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
    };
    table_quant first_table_quant(67, 1, 0, first_matrix);
    table_quants.push_back(first_table_quant);

    //second table_quant
    vector<vector<int>> second_matrix = {
        {0xAA, 0xB4, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xB4, 0xD2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
    };
    table_quant second_table_quant(67, 1, 1, second_matrix);
    table_quants.push_back(second_table_quant);

    for (int ind = 0; ind < table_quants.size(); ind++) {
        CHECK(table_quants[ind] == jpg_decoder.get_table_quant(ind));
    }

    //sof0
    vector<channel> channels;
    channels.push_back({1, 2, 2, 0});
    channels.push_back({2, 1, 1, 1});
    channels.push_back({3, 1, 1, 1});
    sof0 first_sof0(17, 8, 16, 16, 3, channels);

    CHECK(first_sof0 == jpg_decoder.get_sof0(0));

    //dht
    map<string, int> tree_list_dht_1;
    tree_list_dht_1["0"] = 0x03;
    tree_list_dht_1["10"] = 0x02;

    map<string, int> tree_list_dht_2;
    tree_list_dht_2["0"] = 0x01;
    tree_list_dht_2["100"] = 0x00;
    tree_list_dht_2["101"] = 0x12;
    tree_list_dht_2["1100"] = 0x02;
    tree_list_dht_2["1101"] = 0x11;
    tree_list_dht_2["1110"] = 0x31;
    tree_list_dht_2["11110"] = 0x21;

    map<string, int> tree_list_dht_3;
    tree_list_dht_3["0"] = 0x00;
    tree_list_dht_3["10"] = 0x01;

    map<string, int> tree_list_dht_4;
    tree_list_dht_4["0"] = 0x11;
    tree_list_dht_4["10"] = 0x00;
    tree_list_dht_4["110"] = 0x01;

    dht dht_1(0x15, 0, 0, true, tree_list_dht_1);
    dht dht_2(0x1A, 1, 0, true, tree_list_dht_2);
    dht dht_3(0x15, 0, 1, true, tree_list_dht_3);
    dht dht_4(0x16, 1, 1, true, tree_list_dht_4);

    vector<dht> dhts;
    dhts.push_back(dht_1);
    dhts.push_back(dht_2);
    dhts.push_back(dht_3);
    dhts.push_back(dht_4);
    for (int i = 0; i < dhts.size(); i++) {
        CHECK(dhts[i] == jpg_decoder.get_dht(i));
    }

    //sos
    vector<channel_sos> channels_sos;
    channels_sos.push_back({1, 0, 0});
    channels_sos.push_back({2, 1, 1});
    channels_sos.push_back({3, 1, 1});
    sos sos_1(12, 3, channels_sos);

    CHECK(sos_1 == jpg_decoder.get_sos(0));

    //Creatror Matrix
    //creatorMatrix creator_1();
}