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

    //Create matrix Y
    vector<vector<int>> matrix_y_1 = {
        {2, 0, 3, 0, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 0, 0},
        {0, -1, -1, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_y_2 = {
        {-2, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0},
        {0, -1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_y_3 = {
        {3, -1, 1, 0, 0, 0, 0, 0},
        {-1, -2, -1, 0, 0, 0, 0, 0},
        {0, -1, 0, 0, 0, 0, 0, 0},
        {-1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_y_4 = {
        {-1, 2, 2, 1, 0, 0, 0, 0},
        {-1, 0, -1, 0, 0, 0, 0, 0},
        {-1, -1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_y_quant_1 = {
        {320, 0, 300, 0, 0, 0, 0, 0},
        {0, 120, 280, 0, 0, 0, 0, 0},
        {0, -130, -160, 0, 0, 0, 0, 0},
        {140, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_y_quant_2 = {
        {-320, 110, 100, 160, 0, 0, 0, 0},
        {0, 0, 140, 0, 0, 0, 0, 0},
        {0, -130, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_y_quant_3 = {
        {480, -110, 100, 0, 0, 0, 0, 0},
        {-120, -240, -140, 0, 0, 0, 0, 0},
        {0, -130, 0, 0, 0, 0, 0, 0},
        {-140, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_y_quant_4 = {
        {-160, 220, 200, 160, 0, 0, 0, 0},
        {-120, 0, -140, 0, 0, 0, 0, 0},
        {-140, -130, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_y_reverse_cos_1 = {
        {138, 92, 27, -17, -17, 28, 93, 139},
        {136, 82, 5, -51, -55, -8, 61, 111},
        {143, 80, -9, -77, -89, -41, 32, 86},
        {157, 95, 6, -62, -76, -33, 36, 86},
        {147, 103, 37, -12, -21, 11, 62, 100},
        {87, 72, 50, 36, 37, 55, 79, 95},
        {-10, 5, 31, 56, 71, 73, 68, 62},
        {-87, -50, 6, 56, 79, 72, 48, 29}
    };

    vector<vector<int>> matrix_y_reverse_cos_2 = {
        {21, -34, -93, -105, -70, -26, -5, -5},
        {33, -21, -81, -97, -68, -34, -22, -27},
        {49, -4, -64, -83, -64, -44, -46, -60},
        {54, 4, -49, -68, -56, -49, -66, -89},
        {42, 0, -44, -56, -43, -43, -71, -102},
        {13, -19, -49, -47, -28, -29, -61, -96},
        {-20, -44, -59, -43, -15, -11, -44, -80},
        {-42, -60, -67, -42, -6, 0, -32, -68}
    };

    vector<vector<int>> matrix_y_reverse_cos_3 = {
        {-103, -77, -35, 12, 55, 85, 102, 109},
        {-43, -25, 6, 41, 73, 96, 110, 116},
        {31, 38, 50, 66, 81, 95, 105, 110},
        {78, 72, 64, 59, 59, 66, 74, 80},
        {94, 78, 53, 32, 23, 28, 41, 51},
        {107, 83, 47, 18, 8, 18, 39, 55},
        {133, 105, 64, 33, 26, 44, 75, 98},
        {158, 129, 86, 55, 51, 76, 113, 140}
    };

    vector<vector<int>> matrix_y_reverse_cos_4 = {
        {-30, -61, -86, -78, -50, -41, -65, -94},
        {8, -28, -62, -63, -42, -36, -61, -90},
        {66, 20, -27, -42, -31, -29, -53, -81},
        {116, 61, -1, -29, -25, -22, -42, -66},
        {137, 74, 1, -34, -29, -19, -28, -45},
        {125, 59, -18, -55, -43, -20, -14, -21},
        {97, 29, -49, -81, -60, -23, -3, -1},
        {74, 6, -71, -100, -72, -25, 2, 10}
    };

    creatorMatrix creator_matrix_y_1(32, true, true, matrix_y_1, matrix_y_quant_1, matrix_y_reverse_cos_1);
    creatorMatrix creator_matrix_y_2(19, true, true, matrix_y_2, matrix_y_quant_2, matrix_y_reverse_cos_2);
    creatorMatrix creator_matrix_y_3(26, true, true, matrix_y_3, matrix_y_quant_3, matrix_y_reverse_cos_3);
    creatorMatrix creator_matrix_y_4(27, true, true, matrix_y_4, matrix_y_quant_4, matrix_y_reverse_cos_4);

    vector<creatorMatrix> creator_matrix_y;
    creator_matrix_y.push_back(creator_matrix_y_1);
    creator_matrix_y.push_back(creator_matrix_y_2);
    creator_matrix_y.push_back(creator_matrix_y_3);
    creator_matrix_y.push_back(creator_matrix_y_4);

    CHECK(jpg_decoder.get_size_creator_matrix_y() == 4);

    for (int i = 0; i < 4; i++) {
        CHECK(jpg_decoder.get_creator_matrix_y(i) == creator_matrix_y[i]);
    }

    //Create matrix Cb and Cr
    vector<vector<int>> matrix_Cb = {
        {-1, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_Cb_quant = {
        {-170, 0, 0, 0, 0, 0, 0, 0},
        {180, 210, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_Cb_reverse_cos = {
        {60, 52, 38, 20, 0, -18, -32, -40},
        {48, 41, 29, 13, -3, -19, -31, -37},
        {25, 20, 12, 2, -9, -19, -27, -32},
        {-4, -6, -9, -13, -17, -20, -23, -25},
        {-37, -35, -33, -29, -25, -21, -18, -17},
        {-67, -63, -55, -44, -33, -22, -14, -10},
        {-90, -84, -71, -56, -39, -23, -11, -4},
        {-102, -95, -81, -62, -42, -23, -9, -1}
    };

    creatorMatrix creator_matrix_Cb(8, true, true, matrix_Cb, matrix_Cb_quant, matrix_Cb_reverse_cos);
    CHECK(creator_matrix_Cb == jpg_decoder.get_creator_matrix_Cb());

    vector<vector<int>> matrix_Cr = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, -1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_Cr_quant = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {180, -210, 0, 0, 0, 0, 0, 0},
        {240, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    vector<vector<int>> matrix_Cr_reverse_cos = {
        {19, 27, 41, 60, 80, 99, 113, 120},
        {0, 6, 18, 34, 51, 66, 78, 85},
        {-27, -22, -14, -4, 7, 17, 25, 30},
        {-43, -41, -38, -34, -30, -27, -24, -22},
        {-35, -36, -39, -43, -47, -51, -53, -55},
        {-5, -9, -17, -28, -39, -50, -58, -62},
        {32, 26, 14, -1, -18, -34, -46, -53},
        {58, 50, 36, 18, -2, -20, -34, -42}
    };

    creatorMatrix creator_matrix_Cr(12, true, true, matrix_Cr, matrix_Cr_quant, matrix_Cr_reverse_cos);
    CHECK(creator_matrix_Cr == jpg_decoder.get_creator_matrix_Cr());
}