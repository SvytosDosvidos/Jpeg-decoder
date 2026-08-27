#include <catch2/catch_test_macros.hpp>
#include <filesystem>

#include "../decoder.h"

TEST_CASE("First test") {
    CHECK(2 + 2 == 4);
}

TEST_CASE("test_jpg.jpg") {
    Decoder jpg_decoder;

    const std::filesystem::path path =
        std::filesystem::path(TEST_DATA_DIR) / "tests/photos/test_jpg.jpg";

    REQUIRE(std::filesystem::exists(path));

    REQUIRE_NOTHROW(
        jpg_decoder.decode(path.string())
    );

    CHECK(jpg_decoder.get_parser().get_is_open());

    CHECK(jpg_decoder.get_parser().get_size_table_quants() == 2);
    CHECK(jpg_decoder.get_parser().get_size_sof0s() == 1);
    CHECK(jpg_decoder.get_parser().get_size_dhts() == 4);
    CHECK(jpg_decoder.get_parser().get_size_soss() == 1);

    //check table_quants
    std::vector<TableQuant> table_quants;

    //first table_quant
    std::vector<std::vector<int>> first_matrix = {
        {0xA0, 0x6E, 0x64, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF},
        {0x78, 0x78, 0x8C, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF},
        {0x8C, 0x82, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF},
        {0x8C, 0xAA, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xB4, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
    };
    TableQuant first_table_quant(67, 1, 0, first_matrix);
    table_quants.push_back(first_table_quant);

    //second table_quant
    std::vector<std::vector<int>> second_matrix = {
        {0xAA, 0xB4, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xB4, 0xD2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
    };
    TableQuant second_table_quant(67, 1, 1, second_matrix);
    table_quants.push_back(second_table_quant);

    for (int ind = 0; ind < table_quants.size(); ind++) {
        CHECK(table_quants[ind] == jpg_decoder.get_parser().get_table_quant(ind));
    }

    //sof0
    std::vector<Channel> channels;
    channels.push_back({1, 2, 2, 0});
    channels.push_back({2, 1, 1, 1});
    channels.push_back({3, 1, 1, 1});
    Sof0 first_sof0(17, 8, 16, 16, 3, channels);

    CHECK(first_sof0 == jpg_decoder.get_parser().get_sof0(0));

    //dht
    std::map<std::string, int> tree_list_dht_1;
    tree_list_dht_1["0"] = 0x03;
    tree_list_dht_1["10"] = 0x02;

    std::map<std::string, int> tree_list_dht_2;
    tree_list_dht_2["0"] = 0x01;
    tree_list_dht_2["100"] = 0x00;
    tree_list_dht_2["101"] = 0x12;
    tree_list_dht_2["1100"] = 0x02;
    tree_list_dht_2["1101"] = 0x11;
    tree_list_dht_2["1110"] = 0x31;
    tree_list_dht_2["11110"] = 0x21;

    std::map<std::string, int> tree_list_dht_3;
    tree_list_dht_3["0"] = 0x00;
    tree_list_dht_3["10"] = 0x01;

    std::map<std::string, int> tree_list_dht_4;
    tree_list_dht_4["0"] = 0x11;
    tree_list_dht_4["10"] = 0x00;
    tree_list_dht_4["110"] = 0x01;

    Dht dht_1(0x15, 0, 0, true, tree_list_dht_1);
    Dht dht_2(0x1A, 1, 0, true, tree_list_dht_2);
    Dht dht_3(0x15, 0, 1, true, tree_list_dht_3);
    Dht dht_4(0x16, 1, 1, true, tree_list_dht_4);

    std::vector<Dht> dhts;
    dhts.push_back(dht_1);
    dhts.push_back(dht_2);
    dhts.push_back(dht_3);
    dhts.push_back(dht_4);
    for (int i = 0; i < dhts.size(); i++) {
        CHECK(dhts[i] == jpg_decoder.get_parser().get_dht(i));
    }

    //sos
    std::vector<channel_sos> channels_sos;
    channels_sos.push_back({1, 0, 0});
    channels_sos.push_back({2, 1, 1});
    channels_sos.push_back({3, 1, 1});
    Sos sos_1(12, 3, channels_sos);

    CHECK(sos_1 == jpg_decoder.get_parser().get_sos(0));

    //Create matrix Y
    std::vector<std::vector<int>> matrix_y_1 = {
        {2, 0, 3, 0, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 0, 0, 0},
        {0, -1, -1, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_y_2 = {
        {-2, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0},
        {0, -1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_y_3 = {
        {3, -1, 1, 0, 0, 0, 0, 0},
        {-1, -2, -1, 0, 0, 0, 0, 0},
        {0, -1, 0, 0, 0, 0, 0, 0},
        {-1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_y_4 = {
        {-1, 2, 2, 1, 0, 0, 0, 0},
        {-1, 0, -1, 0, 0, 0, 0, 0},
        {-1, -1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_y_quant_1 = {
        {320, 0, 300, 0, 0, 0, 0, 0},
        {0, 120, 280, 0, 0, 0, 0, 0},
        {0, -130, -160, 0, 0, 0, 0, 0},
        {140, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_y_quant_2 = {
        {-320, 110, 100, 160, 0, 0, 0, 0},
        {0, 0, 140, 0, 0, 0, 0, 0},
        {0, -130, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_y_quant_3 = {
        {480, -110, 100, 0, 0, 0, 0, 0},
        {-120, -240, -140, 0, 0, 0, 0, 0},
        {0, -130, 0, 0, 0, 0, 0, 0},
        {-140, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_y_quant_4 = {
        {-160, 220, 200, 160, 0, 0, 0, 0},
        {-120, 0, -140, 0, 0, 0, 0, 0},
        {-140, -130, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_y_reverse_cos_1 = {
        {138, 93, 28, -18, -18, 28, 94, 139},
        {136, 82, 5, -51, -56, -8, 61, 112},
        {143, 81, -9, -78, -89, -41, 33, 86},
        {157, 96, 7, -63, -77, -33, 36, 87},
        {148, 103, 38, -12, -21, 11, 63, 101},
        {87, 72, 51, 36, 38, 55, 79, 96},
        {-11, 6, 32, 56, 71, 73, 68, 63},
        {-87, -50, 7, 56, 80, 73, 49, 29}
    };

    std::vector<std::vector<int>> matrix_y_reverse_cos_2 = {
        {21, -34, -94, -106, -70, -26, -6, -5},
        {34, -22, -82, -98, -69, -34, -22, -27},
        {49, -5, -64, -84, -65, -44, -47, -60},
        {55, 5, -50, -69, -57, -49, -66, -89},
        {42, 0, -44, -56, -44, -44, -71, -102},
        {13, -20, -49, -48, -29, -29, -62, -96},
        {-20, -44, -59, -44, -15, -12, -45, -81},
        {-42, -61, -67, -42, -7, 0, -32, -69}
    };

    std::vector<std::vector<int>> matrix_y_reverse_cos_3 = {
        {-103, -78, -35, 13, 55, 85, 102, 109},
        {-44, -25, 6, 42, 74, 97, 111, 117},
        {31, 38, 51, 66, 82, 96, 105, 110},
        {79, 73, 65, 59, 60, 66, 75, 81},
        {95, 78, 53, 32, 23, 29, 41, 51},
        {108, 84, 48, 18, 8, 19, 40, 56},
        {134, 106, 65, 33, 26, 45, 76, 98},
        {159, 129, 87, 55, 52, 76, 113, 140}
    };

    std::vector<std::vector<int>> matrix_y_reverse_cos_4 = {
        {-31, -61, -87, -79, -51, -41, -65, -95},
        {8, -28, -63, -63, -42, -36, -61, -91},
        {66, 21, -28, -43, -31, -29, -53, -81},
        {116, 61, -2, -30, -25, -23, -42, -66},
        {137, 75, 1, -34, -30, -20, -29, -45},
        {126, 59, -19, -55, -44, -20, -15, -22},
        {97, 29, -50, -82, -61, -23, -4, -2},
        {75, 6, -72, -100, -72, -26, 3, 10}
    };

    CreatorMatrix creator_matrix_y_1(32, true, true, matrix_y_1, matrix_y_quant_1, matrix_y_reverse_cos_1);
    CreatorMatrix creator_matrix_y_2(19, true, true, matrix_y_2, matrix_y_quant_2, matrix_y_reverse_cos_2);
    CreatorMatrix creator_matrix_y_3(26, true, true, matrix_y_3, matrix_y_quant_3, matrix_y_reverse_cos_3);
    CreatorMatrix creator_matrix_y_4(27, true, true, matrix_y_4, matrix_y_quant_4, matrix_y_reverse_cos_4);

    std::vector<CreatorMatrix> creator_matrix_y;
    creator_matrix_y.push_back(creator_matrix_y_1);
    creator_matrix_y.push_back(creator_matrix_y_2);
    creator_matrix_y.push_back(creator_matrix_y_3);
    creator_matrix_y.push_back(creator_matrix_y_4);

    CHECK(jpg_decoder.get_size_creator_matrix_y() == 4);

    for (int i = 0; i < 4; i++) {
        CHECK(jpg_decoder.get_creator_matrix_y(i) == creator_matrix_y[i]);
    }

    //Create matrix Cb and Cr
    std::vector<std::vector<int>> matrix_Cb = {
        {-1, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_Cb_quant = {
        {-170, 0, 0, 0, 0, 0, 0, 0},
        {180, 210, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_Cb_reverse_cos = {
        {60, 53, 39, 20, 0, -19, -33, -41},
        {48, 42, 29, 14, -3, -19, -31, -38},
        {25, 21, 13, 2, -9, -20, -28, -32},
        {-5, -7, -9, -13, -17, -21, -24, -25},
        {-38, -36, -33, -29, -25, -22, -19, -17},
        {-68, -63, -55, -45, -33, -23, -15, -10},
        {-91, -84, -72, -56, -39, -23, -11, -5},
        {-103, -95, -81, -63, -42, -24, -10, -2}
    };

    CreatorMatrix creator_matrix_Cb(8, true, true, matrix_Cb, matrix_Cb_quant, matrix_Cb_reverse_cos);
    CHECK(creator_matrix_Cb == jpg_decoder.get_creator_matrix_Cb());

    std::vector<std::vector<int>> matrix_Cr = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, -1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_Cr_quant = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {180, -210, 0, 0, 0, 0, 0, 0},
        {240, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> matrix_Cr_reverse_cos = {
        {20, 28, 42, 60, 80, 99, 113, 121},
        {0, 6, 18, 34, 51, 67, 79, 86},
        {-27, -23, -15, -4, 7, 18, 26, 30},
        {-43, -42, -39, -35, -31, -27, -24, -23},
        {-35, -37, -40, -43, -47, -51, -54, -55},
        {-5, -10, -18, -28, -40, -50, -58, -63},
        {33, 26, 14, -2, -19, -34, -47, -53},
        {58, 51, 37, 18, -2, -21, -35, -43}
    };

    CreatorMatrix creator_matrix_Cr(12, true, true, matrix_Cr, matrix_Cr_quant, matrix_Cr_reverse_cos);
    CHECK(creator_matrix_Cr == jpg_decoder.get_creator_matrix_Cr());

    //Test Image RGB
    std::vector<std::vector<int>> matrix_r = {
        {255, 249, 195, 149, 169, 215, 255, 255},
        {255, 238, 172, 116, 131, 179, 255, 255},
        {255, 209, 127,  58,  64, 112, 209, 255},
        {255, 224, 143,  73,  76, 120, 212, 255},
        {217, 193, 134,  84,  86, 118, 185, 223},
        {177, 162, 147, 132, 145, 162, 201, 218},
        { 57,  74, 101, 125, 144, 146, 147, 142},
        {  0,  18,  76, 125, 153, 146, 128, 108}
    };

    std::vector<std::vector<int>> matrix_g = {
        {220, 186, 118,  72,  67, 113, 172, 205},
        {220, 175,  95,  39,  29,  77, 139, 190},
        {238, 192, 100,  31,  16,  64, 132, 185},
        {238, 207, 116,  46,  28,  72, 135, 186},
        {255, 242, 175, 125, 113, 145, 193, 231},
        {226, 211, 188, 173, 172, 189, 209, 226},
        {149, 166, 192, 216, 230, 232, 225, 220},
        { 73, 110, 167, 216, 239, 232, 206, 186}
    };

    std::vector<std::vector<int>> matrix_b = {
        {255, 255, 250, 204, 179, 225, 255, 255},
        {255, 255, 227, 171, 141, 189, 224, 255},
        {255, 255, 193, 124,  90, 138, 186, 239},
        {255, 255, 209, 139, 102, 146, 189, 240},
        {255, 255, 203, 153, 130, 162, 195, 233},
        {255, 244, 216, 201, 189, 206, 211, 228},
        {108, 125, 148, 172, 183, 185, 173, 168},
        { 32,  69, 123, 172, 192, 185, 154, 134}
    };

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            CHECK(matrix_r[i][j] == jpg_decoder.get_image().get_el_matrix_r(i,j));
            CHECK(matrix_g[i][j] == jpg_decoder.get_image().get_el_matrix_g(i,j));
            CHECK(matrix_b[i][j] == jpg_decoder.get_image().get_el_matrix_b(i,j));
        }
    }
}