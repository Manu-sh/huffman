#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <common.hpp>
#include <HuffmanTree.hpp>
#include <Histogram.hpp>
#include <SymbolTable.hpp>
#include <encoder/Encoder.hpp>
#include <decoder/Decoder.hpp>
#include <bitarray/BitArray.hpp>

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <iostream>
#include <string>
#include <cstdint>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

#include "huffman_code_test.hpp"
#include "bitarray_test.hpp"
#include "old_hafe_test.hpp"
#include "encoder_decoder_test.hpp"
#include <HuffmanCode.hpp>

using namespace std;

TEST_CASE("testing huffman on strings") {

    static const auto try_decode = [] (const std::string &str) -> void {

        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        SymbolTable st{tree};

        REQUIRE(st.longest().bit_length() < HuffmanCode::MAX_LENGTH);

        //tree.print();
        REQUIRE(st.longest().bit_length() == tree.height());
        const std::vector<HuffmanCode> &symbol_table = st.borrow();

        auto shp_encoded = Encoder::encode(st, str);
        auto shp_decoded = Decoder::decode(st, *shp_encoded);
        REQUIRE(str == *shp_decoded);
    };

    try_decode("il mio angolo di cielo e un triangolo di pelo");
    try_decode("aaaaabbbbbbbbbccccccccccccdddddddddddddeeeeeeeeeeeeeeeefffffffffffffffffffffffffffffffffffffffffffff");
    try_decode("\x1dmiao miao miao miaooo");
    try_decode("BPPRRRRRRBPGRGPR");
    try_decode("abccdddeeeeeffffffffggggggggggggghhhhhhhhhhhhhhhhhhhhh");
    try_decode("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbccccccccccccddddddddddddddddddddddddeeeeeeeeefffff");

    //try_decode(file_content("../../data/divina_commedia.txt"));


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
