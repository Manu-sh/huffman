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
#include <HuffmanCode.hpp>

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


// skipping the zero return every other element of fibonacci until max_times is reached or fibo(n) > max_fibo
template <typename T>
static std::vector<T> fibonacci_gen(uint32_t from, uint32_t to, uint64_t max_fibo = std::numeric_limits<T>::max()) {

    if (max_fibo == std::numeric_limits<uint64_t>::max())
        --max_fibo;

    std::vector<T> numbers{1, 1};

    uint64_t a = 1, b = 1;
    uint64_t cur_fibo = a + b;

    for (uint32_t i = from; i < to; ++i, cur_fibo = a + b) {
        if (cur_fibo >= max_fibo)
            return numbers;

        numbers.push_back(cur_fibo);
        a = b;
        b = cur_fibo;
    }

    return numbers;
}


#include <algorithm>
TEST_CASE("testing ") {

    {

        // fibonacci tends to produce the worst case scenario in huffman tree(s)
        // https://stackoverflow.com/questions/19883086/optimal-huffman-code-for-fibonacci-numbers#19893487
        // https://github.com/ahmedss33/Introduction-to-Algorithms-Solutions/blob/master/C16-Greedy-Algorithms/16.3.md



        /*
        TODO: praticamente l'oveflow ha fatto impazzire huffman che non si comporta più come dovrebbe e crea una lista vera e propria,
         il limite dell'uint32_t ha senso solo se anche l'algoritmo lo rispetta.
        */
        Histogram fake_freq;
        //memset(fake_freq.m_frequency, 1, sizeof(fake_freq.m_frequency));


        for (int i = 0; i < 256; ++i) {
            fake_freq.m_frequency[i] = std::numeric_limits<uint32_t>::max(); // memset 1
            cout << ((void *)(long)i) << " " << fake_freq.m_frequency[i] << endl;
        }


        auto vc = fibonacci_gen<uint32_t>(2, 256);
        for (const auto e : vc) {
            static int i = 2;
            cout << "i " << i << " e: " << e << endl;
            fake_freq.m_frequency[i] = e;
            REQUIRE(i++ < 256);
        }


        /*

        fibonacci_gen([&fake_freq] (uint32_t fibo, uint8_t idx) {
            fake_freq.m_frequency[idx] = fibo;
        }, 2, 256, std::numeric_limits<uint32_t>::max());
        */

        for (int i = 0; i < 256; ++i) {
            cout << ((void *)(long)i) << " " << fake_freq.m_frequency[i] << endl;
        }

        REQUIRE(huffman_code_max_length<uint32_t>() == 48);
        REQUIRE(huffman_code_max_length<uint16_t>() == 25);
        REQUIRE(huffman_code_max_length<uint8_t>()  == 14);

//exit(0);

        // TODO: the maximum length for huffman codes with the freq limited to u32 is 48 bit
        auto tree = HuffmanTree(fake_freq);

        SymbolTable st{tree};
        st.print();

        const auto &v = st.borrow();
        auto f_max = std::max_element(v.begin(), v.end(), [] (const HuffmanCode &a, const HuffmanCode &b) -> bool {
            return a.bit_length() < b.bit_length();
        })->bit_length();

        //cout << "here: " << f_max << endl;
        REQUIRE(huffman_code_max_length<uint32_t>() == f_max);

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
