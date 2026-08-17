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
    CHECK(jpg_decoder.get_size_dhts() == 4);
    CHECK(jpg_decoder.get_size_sof0s() == 1);

    cout << 1 << "\n";

    /*vector<table_quant> table_quants;
    vector<vector<int>> matrix = {
        {0xA0, 0x6E, 0x64, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF},
        {0x78, 0x78, 0x8C, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF},
        {0x8C, 0x82, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF},
        {0x8C, 0xAA, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xB4, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
    };

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << jpg_decoder.get_table_quant(0).get_el_matrix(i,j) << " ";
        }
        cout << "\n";
    }

    table_quant first_table_quant(67, 1, 0, matrix);
    table_quants.push_back(first_table_quant);

    CHECK(first_table_quant == jpg_decoder.get_table_quant(0));*/
}