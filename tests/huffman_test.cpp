#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <encode_decode.hpp>
#include <HuffmanTree.hpp>
#include <Histogram.hpp>
#include <bitarray/BitArray.hpp>

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <iostream>
#include <string>
#include <cstdint>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

#include "bitarray_test.hpp"

using namespace std;

TEST_CASE("testing huffman on strings") {

    static const auto try_decode = [] (const std::string &str) -> void {
        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        auto shp_sym_tab = tree.symbol_table();
        std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

        REQUIRE(str == decode(symbol_table, encode(symbol_table, str)));
    };

    try_decode("il mio angolo di cielo e un triangolo di pelo");
    try_decode("aaaaabbbbbbbbbccccccccccccdddddddddddddeeeeeeeeeeeeeeeefffffffffffffffffffffffffffffffffffffffffffff");
    try_decode("\x1dmiao miao miao miaooo");
    try_decode("BPPRRRRRRBPGRGPR");
    try_decode("abccdddeeeeeffffffffggggggggggggghhhhhhhhhhhhhhhhhhhhh");
    try_decode("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbccccccccccccddddddddddddddddddddddddeeeeeeeeefffff");

}

int main(int argc, char *argv[]) {

    doctest::Context context;
    context.applyCommandLine(argc, argv);

    int res = context.run(); // run doctest

    // important - query flags (and --exit) rely on the user doing this
    if (context.shouldExit()) {
        // propagate the result of the tests
        return res;
    }
}
