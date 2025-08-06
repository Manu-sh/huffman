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
#include <limits>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

#include "bitarray_test.hpp"
#include "old_hafe_test.hpp"
#include "encoder_decoder_test.hpp"

using namespace std;

TEST_CASE("testing huffman on strings") {

    static const auto try_decode = [] (const std::string &str) -> void {

        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        SymbolTable st{tree};

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


TEST_CASE("testing ") {

    {
        Histogram fake_freq;

        for (int i = 0; i < 128; ++i) {
            //fake_freq.m_frequency[i] = std::numeric_limits<decltype(fake_freq.m_frequency[0])>::max() - 1;
            //fake_freq.m_frequency[i] = std::numeric_limits<uint32_t>::max() - 1;
            fake_freq.m_frequency[i] = (i + 1) * 1;
        }

        //fake_freq.m_frequency[0] = 1;
        fake_freq.m_frequency[0] = std::numeric_limits<uint32_t>::max() - 1;
        auto tree = HuffmanTree(fake_freq);

        SymbolTable st{tree};
        st.print();
    }


    {
        // compress
        std::string str = file_content("../../data/divina_commedia.txt");
        ShannonHistogram freq{(uint8_t *) str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        SymbolTable st{tree};
        //st.print();
        //freq.dump_entry(0xb6);

    }


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
