cmake_minimum_required(VERSION 3.16)
project(jpg_decoder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
        jpg_decoder
        files/main.cpp
        files/parser.cpp
        files/parser.h
        files/section.cpp
        files/section.h
        files/markers.cpp
        files/markers.h
        files/image.cpp
        files/image.h
        files/creator_matrix.cpp
        files/creator_matrix.h
        files/decoder.cpp
        files/decoder.h
        files/creator_marker.cpp
        files/creator_marker.h
)

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.12.0
        GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(Catch2)

add_executable(
        jpg_decoder_tests

        files/decoder.cpp
        files/decoder.h
        files/parser.cpp
        files/parser.h
        files/section.cpp
        files/section.h
        files/markers.cpp
        files/markers.h
        files/image.cpp
        files/image.h
        files/creator_matrix.cpp
        files/creator_matrix.h

        files/tests/tests.cpp
)

target_link_libraries(
        jpg_decoder_tests
        PRIVATE Catch2::Catch2WithMain
)

target_compile_definitions(
        jpg_decoder_tests
        PRIVATE TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/files"
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

catch_discover_tests(jpg_decoder_tests)