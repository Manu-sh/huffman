#pragma once
#include <doctest/doctest.h>

#include <hafe_file_format/Hafe.hpp>
#include <common.hpp>
#include <HuffmanTree.hpp>
#include <Histogram.hpp>
#include <SymbolTable.hpp>
#include <bitarray/BitArray.hpp>
#include <encoder/Encoder.hpp>
#include <decoder/Decoder.hpp>


DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <iostream>
#include <string>
#include <cstdint>
#include <memory>

#include <ctime>
#include <cstdlib>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

using namespace std;

TEST_CASE("testing Hafe::calc_symbol_table_disk_size()") {

    { // single row test
        SymbolTable fake_st{std::make_shared<vector<BitArray>>(256)};
        auto &fake_symtable = fake_st.mut();
        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_st) == 0);

        srand(time(0));

        uint8_t sym = rand() % 256;
        auto &row = fake_symtable[sym];

        row.push_back(1); // 1° bit

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_st) ==
            sizeof(uint8_t) + sizeof(uint16_t) + row.effective_byte_size()
        );

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_st) ==
            1+2+1
        );

        // 7 bit
        row.push_back(1).push_back(1).push_back(1);
        row.push_back(1).push_back(1).push_back(1).push_back(1);

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_st) ==
            sizeof(uint8_t) + sizeof(uint16_t) + row.effective_byte_size()
        );

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_st) ==
            1+2+1 // 4
        );

        row.push_back(1); // 9° bit, now is larger the row require 2 byte

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_st) ==
            sizeof(uint8_t) + sizeof(uint16_t) + row.effective_byte_size()
        );

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_st) == 5);
    }

    {
        /*
            real case test: "BPPRRRRRRBPGRGPR"

            'B'  | 110
            'G'  | 111
            'P'  | 10
            'R'  | 0

            format will be stored on disk as follow:

                | uint8_t | uint16_t         | uint8_t[]   |
                ----------|------------------|-------------
                | symbol  | bit_length(code) | prefix code |

         */

        SymbolTable fake_st{std::make_shared<vector<BitArray>>(256)};
        auto &fake_symtable = fake_st.mut();

        fake_symtable['B'].push_back(1).push_back(1).push_back(0);
        fake_symtable['G'].push_back(1).push_back(1).push_back(1);
        fake_symtable['P'].push_back(1).push_back(0);
        fake_symtable['R'].push_back(0);

        // every row take less than 8 bit so bitstream fit into a single byte
        REQUIRE(fake_symtable['B'].effective_byte_size() == sizeof(uint8_t));
        REQUIRE(fake_symtable['G'].effective_byte_size() == sizeof(uint8_t));
        REQUIRE(fake_symtable['P'].effective_byte_size() == sizeof(uint8_t));
        REQUIRE(fake_symtable['R'].effective_byte_size() == sizeof(uint8_t));

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_st) ==
            (sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t)) * strlen("BGPR") // size of Rows x Column
        );

    }

}



TEST_CASE("testing .hafe compress & decompress") {

    // im sorry for this-copy paste but im bit tired and are just tests

    // "../../data/divina_commedia.txt", "../../data/divina_commedia.hafe"
    const static auto &test_compress_old = [] (const std::string &file_uncompressed, const std::string &out) {

        // compress
        std::string str = file_content(file_uncompressed);
        //Histogram freq{(uint8_t *)str.data(), str.length()};
        ShannonHistogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        SymbolTable st{tree};

        const auto &symbol_table = st.borrow();
        {
            ShannonHistogram recalc{symbol_table, str.length()};
            for (auto [s, f] : recalc) {
                REQUIRE(freq.probability(s) == recalc.probability(s));
                REQUIRE(freq.information(s) == recalc.information(s));
                REQUIRE(freq[s] == recalc[s]);
                REQUIRE(freq.information(s) == recalc.information(s));
                REQUIRE(freq.avg_bit_per_symbol() == recalc.avg_bit_per_symbol());
                REQUIRE(freq.total_bits() == recalc.total_bits());
                REQUIRE(freq.total_bytes() == recalc.total_bytes());
            }
        }


        std::shared_ptr<BitArray> shp_bitstream = Encoder::encode(st, str);

        REQUIRE(st.share() != nullptr);
        REQUIRE(shp_bitstream != nullptr);

        auto where = ofstream_open(out.c_str());
        Hafe hafe{st, shp_bitstream};
        hafe.write(where);
    };


    // "../../data/divina_commedia.hafe", "../../data/divina_commedia.txt"
    // "../../data/divina_commedia.hafe", "/tmp/divina_commedia.txt"
    const static auto &test_decompress_old = [] (const std::string &file_compressed, const std::string &file_uncompressed) { // decompress

        auto where = ifstream_open(file_compressed.c_str());
        Hafe hafe{where};

        auto st = hafe.symbol_table();
        const auto &symbol_table = st.borrow();

        REQUIRE(st.share() != nullptr);
        // SymbolTable::print_symbol_table(symbol_table);

        auto bt = hafe.bitstream();
        auto shp_bitstream = bt.share();

        REQUIRE(shp_bitstream != nullptr);

        //REQUIRE(shp_bitstream->effective_byte_size() == 331643); // numfmt --to=iec 331643 -> 324K solo per divina commedia.txt
        auto shp_str = Decoder::decode(st, bt.borrow());
        REQUIRE(*shp_str == file_content(file_uncompressed));
    };

    // "../../data/divina_commedia.txt"
    const static auto &full_test = [] (const std::string &file_path) {
        test_compress_old(file_path, "/tmp/test.hafe");
        test_decompress_old("/tmp/test.hafe", file_path);
    };


    const static auto &compress_decompress_string_full = [] (const std::string &sentence) {

        { // compress
            Histogram freq{(uint8_t *)sentence.data(), sentence.length()};

            auto tree = HuffmanTree(freq);
            SymbolTable st{tree};

            const auto &symbol_table = st.borrow();
            REQUIRE(st.share() != nullptr);

            std::shared_ptr<BitArray> shp_bitstream = Encoder::encode(st, sentence);

            REQUIRE(shp_bitstream != nullptr);

            //REQUIRE(Hafe::calc_symbol_table_disk_size(st) == 60);

            auto where = ofstream_open("/tmp/sentence.hafe");
            Hafe hafe{st, shp_bitstream};
            hafe.write(where);
        }

        { // decompress
            auto where = ifstream_open("/tmp/sentence.hafe");
            Hafe hafe{where};

            auto st = hafe.symbol_table();
            const auto &symbol_table = st.borrow();

            REQUIRE(st.share() != nullptr);
            // SymbolTable::print_symbol_table(symbol_table);

            auto bt = hafe.bitstream();
            auto shp_bitstream = bt.share();

            REQUIRE(shp_bitstream != nullptr);

            auto shp_str = Decoder::decode(st, bt.borrow());
            REQUIRE(*shp_str == sentence);
        }

    };

    for (const auto &s : {
        "../../data/divina_commedia.txt",
        //"../../data/lorem_ipsum.txt",
        "../../data/mandelbrot.pnm"
    })
        full_test(s);

        for (const auto &s : {
            "il mio angolo di cielo e un triangolo di pelo",
            "aaaaabbbbbbbbbccccccccccccdddddddddddddeeeeeeeeeeeeeeeefffffffffffffffffffffffffffffffffffffffffffff",
            "\x1dmiao miao miao miaooo",
            "BPPRRRRRRBPGRGPR",
            "abccdddeeeeeffffffffggggggggggggghhhhhhhhhhhhhhhhhhhhh",
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbccccccccccccddddddddddddddddddddddddeeeeeeeeefffff"
        })
            compress_decompress_string_full(s);


        //auto out = ofstream_open("../../data/mandelbrot.pnm.out");
        //out.write((char *)str.data(), str.length());
}
