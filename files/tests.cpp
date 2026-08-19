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

    /*
    Для удобства написания тестов, делать их не вручную, а как в статье
    1 - поворот направо, 0 - налево, к примеру, и так однозначно задать каждую вершину
    и сравнить, что они совпадают, и смотреть только какие-то ключевые, а во всех
    остальных, которые как бы неважны, посмотреть, что там лежит -1

    И сделать проверку на то, что дерево может быть не построено во все

    tree* start_tree_1 = new tree({-1, nullptr, nullptr});
    start_tree_1->l = new tree({0x, nullptr, nullptr});*/
}