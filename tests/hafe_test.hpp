#pragma once
#include <doctest/doctest.h>

#include <hafe_file_format/Hafe.hpp>
#include <common.hpp>
#include <encode_decode.hpp>
#include <HuffmanTree.hpp>
#include <Histogram.hpp>
#include <bitarray/BitArray.hpp>


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
        auto shp_fake_symtable = std::make_shared<vector<BitArray>>(256);
        auto &fake_symtable = *shp_fake_symtable;

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_symtable) == 0);

        srand(time(0));

        uint8_t sym = rand() % 256;
        auto &row = fake_symtable[sym];

        row.push_back(1); // 1° bit

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_symtable) ==
            sizeof(uint8_t) + sizeof(uint16_t) + row.effective_byte_size()
        );

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_symtable) ==
            1+2+1
        );

        // 7 bit
        row.push_back(1).push_back(1).push_back(1);
        row.push_back(1).push_back(1).push_back(1).push_back(1);

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_symtable) ==
            sizeof(uint8_t) + sizeof(uint16_t) + row.effective_byte_size()
        );

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_symtable) ==
            1+2+1 // 4
        );

        row.push_back(1); // 9° bit, now is larger the row require 2 byte

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_symtable) ==
            sizeof(uint8_t) + sizeof(uint16_t) + row.effective_byte_size()
        );

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_symtable) == 5);
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

        auto shp_fake_symtable = std::make_shared<vector<BitArray>>(256);
        auto &fake_symtable = *shp_fake_symtable;

        fake_symtable['B'].push_back(1).push_back(1).push_back(0);
        fake_symtable['G'].push_back(1).push_back(1).push_back(1);
        fake_symtable['P'].push_back(1).push_back(0);
        fake_symtable['R'].push_back(0);

        // every row take less than 8 bit so bitstream fit into a single byte
        REQUIRE(fake_symtable['B'].effective_byte_size() == sizeof(uint8_t));
        REQUIRE(fake_symtable['G'].effective_byte_size() == sizeof(uint8_t));
        REQUIRE(fake_symtable['P'].effective_byte_size() == sizeof(uint8_t));
        REQUIRE(fake_symtable['R'].effective_byte_size() == sizeof(uint8_t));

        REQUIRE(Hafe::calc_symbol_table_disk_size(fake_symtable) ==
            (sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t)) * strlen("BGPR") // size of Rows x Column
        );

    }

}


TEST_CASE("testing .hafe compress & decompress") {

    // im sorry for this-copy paste but im bit tired and are just tests

    { // compress
        std::string str{"il mio angolo di cielo e un triangolo di pelo"};
        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        auto shp_sym_tab = tree.symbol_table();
        std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

        std::shared_ptr<BitArray> shp_bitstream = make_shared<BitArray>(encode(symbol_table, str));

        REQUIRE(shp_sym_tab != nullptr);
        REQUIRE(shp_bitstream != nullptr);

        REQUIRE(Hafe::calc_symbol_table_disk_size(symbol_table) == 60);

        auto where = ofstream_open("/home/user/huffman/data/test.hafe");
        Hafe hafe{shp_sym_tab, shp_bitstream};
        hafe.write(where);
    }

    { // decompress
        auto where = ifstream_open("/home/user/huffman/data/test.hafe");
        Hafe hafe{where};

        auto shp_sym_tab = hafe.symbol_table();
        const auto &symbol_table = *shp_sym_tab;

        REQUIRE(shp_sym_tab != nullptr);

        for (unsigned i = 0; i < symbol_table.size(); ++i) {

            const auto &bit_v = symbol_table[i];
            if (bit_v.empty()) continue;

            const std::string &bit_str{bit_v};
            printf(std::isprint(i) ? "'%c'  | %s\n" : "%#02x | %s\n", i, bit_str.c_str()); // this is so bad.. but it's just 4 dbg :)
        }

        auto shp_bitstream = hafe.bitstream();
        REQUIRE(shp_bitstream != nullptr);

        auto str = decode(symbol_table, *shp_bitstream);
        REQUIRE(str == "il mio angolo di cielo e un triangolo di pelo");
    }


    { // compress
        std::string str = file_content("/home/user/huffman/data/divina_commedia.txt");
        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        auto shp_sym_tab = tree.symbol_table();
        std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

        std::shared_ptr<BitArray> shp_bitstream = make_shared<BitArray>(encode(symbol_table, str));

        REQUIRE(shp_sym_tab != nullptr);
        REQUIRE(shp_bitstream != nullptr);

        //REQUIRE(Hafe::calc_symbol_table_disk_size(symbol_table) == 60);

        auto where = ofstream_open("/home/user/huffman/data/divina_commedia.hafe");
        Hafe hafe{shp_sym_tab, shp_bitstream};
        hafe.write(where);
    }

    { // decompress
        auto where = ifstream_open("/home/user/huffman/data/divina_commedia.hafe");
        Hafe hafe{where};

        auto shp_sym_tab = hafe.symbol_table();
        const auto &symbol_table = *shp_sym_tab;

        REQUIRE(shp_sym_tab != nullptr);

        for (unsigned i = 0; i < symbol_table.size(); ++i) {

            const auto &bit_v = symbol_table[i];
            if (bit_v.empty()) continue;

            const std::string &bit_str{bit_v};
            printf(std::isprint(i) ? "'%c'  | %s\n" : "%#02x | %s\n", i, bit_str.c_str()); // this is so bad.. but it's just 4 dbg :)
        }

        auto shp_bitstream = hafe.bitstream();
        REQUIRE(shp_bitstream != nullptr);

        REQUIRE(shp_bitstream->effective_byte_size() == 331643); // numfmt --to=iec 331643 -> 324K
        auto str = decode(symbol_table, *shp_bitstream);
        REQUIRE(str == file_content("/home/user/huffman/data/divina_commedia.txt"));
    }


    { // compress
        std::string str{"aaaaabbbbbbbbbccccccccccccdddddddddddddeeeeeeeeeeeeeeeefffffffffffffffffffffffffffffffffffffffffffff"};
        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        auto shp_sym_tab = tree.symbol_table();
        std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

        std::shared_ptr<BitArray> shp_bitstream = make_shared<BitArray>(encode(symbol_table, str));

        REQUIRE(shp_sym_tab != nullptr);
        REQUIRE(shp_bitstream != nullptr);

        //REQUIRE(Hafe::calc_symbol_table_disk_size(symbol_table) == 60);

        auto where = ofstream_open("/home/user/huffman/data/test-2.hafe");
        Hafe hafe{shp_sym_tab, shp_bitstream};
        hafe.write(where);
    }

    { // decompress
        auto where = ifstream_open("/home/user/huffman/data/test-2.hafe");
        Hafe hafe{where};

        auto shp_sym_tab = hafe.symbol_table();
        const auto &symbol_table = *shp_sym_tab;

        REQUIRE(shp_sym_tab != nullptr);

        for (unsigned i = 0; i < symbol_table.size(); ++i) {

            const auto &bit_v = symbol_table[i];
            if (bit_v.empty()) continue;

            const std::string &bit_str{bit_v};
            printf(std::isprint(i) ? "'%c'  | %s\n" : "%#02x | %s\n", i, bit_str.c_str()); // this is so bad.. but it's just 4 dbg :)
        }

        auto shp_bitstream = hafe.bitstream();
        REQUIRE(shp_bitstream != nullptr);

        auto str = decode(symbol_table, *shp_bitstream);
        REQUIRE(str == "aaaaabbbbbbbbbccccccccccccdddddddddddddeeeeeeeeeeeeeeeefffffffffffffffffffffffffffffffffffffffffffff");
    }


    { // compress
        std::string str{"\x1dmiao miao miao miaooo"};
        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        auto shp_sym_tab = tree.symbol_table();
        std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

        std::shared_ptr<BitArray> shp_bitstream = make_shared<BitArray>(encode(symbol_table, str));

        REQUIRE(shp_sym_tab != nullptr);
        REQUIRE(shp_bitstream != nullptr);

        //REQUIRE(Hafe::calc_symbol_table_disk_size(symbol_table) == 60);

        auto where = ofstream_open("/home/user/huffman/data/test-3.hafe");
        Hafe hafe{shp_sym_tab, shp_bitstream};
        hafe.write(where);
    }

    { // decompress
        auto where = ifstream_open("/home/user/huffman/data/test-3.hafe");
        Hafe hafe{where};

        auto shp_sym_tab = hafe.symbol_table();
        const auto &symbol_table = *shp_sym_tab;

        REQUIRE(shp_sym_tab != nullptr);

        for (unsigned i = 0; i < symbol_table.size(); ++i) {

            const auto &bit_v = symbol_table[i];
            if (bit_v.empty()) continue;

            const std::string &bit_str{bit_v};
            printf(std::isprint(i) ? "'%c'  | %s\n" : "%#02x | %s\n", i, bit_str.c_str()); // this is so bad.. but it's just 4 dbg :)
        }

        auto shp_bitstream = hafe.bitstream();
        REQUIRE(shp_bitstream != nullptr);

        auto str = decode(symbol_table, *shp_bitstream);
        REQUIRE(str == "\x1dmiao miao miao miaooo");
    }


    { // compress
        std::string str{"BPPRRRRRRBPGRGPR"};
        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        auto shp_sym_tab = tree.symbol_table();
        std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

        std::shared_ptr<BitArray> shp_bitstream = make_shared<BitArray>(encode(symbol_table, str));

        REQUIRE(shp_sym_tab != nullptr);
        REQUIRE(shp_bitstream != nullptr);

        //REQUIRE(Hafe::calc_symbol_table_disk_size(symbol_table) == 60);

        auto where = ofstream_open("/home/user/huffman/data/test-4.hafe");
        Hafe hafe{shp_sym_tab, shp_bitstream};
        hafe.write(where);
    }

    { // decompress
        auto where = ifstream_open("/home/user/huffman/data/test-4.hafe");
        Hafe hafe{where};

        auto shp_sym_tab = hafe.symbol_table();
        const auto &symbol_table = *shp_sym_tab;

        REQUIRE(shp_sym_tab != nullptr);

        for (unsigned i = 0; i < symbol_table.size(); ++i) {

            const auto &bit_v = symbol_table[i];
            if (bit_v.empty()) continue;

            const std::string &bit_str{bit_v};
            printf(std::isprint(i) ? "'%c'  | %s\n" : "%#02x | %s\n", i, bit_str.c_str()); // this is so bad.. but it's just 4 dbg :)
        }

        auto shp_bitstream = hafe.bitstream();
        REQUIRE(shp_bitstream != nullptr);

        auto str = decode(symbol_table, *shp_bitstream);
        REQUIRE(str == "BPPRRRRRRBPGRGPR");
    }


    { // compress
        std::string str{"abccdddeeeeeffffffffggggggggggggghhhhhhhhhhhhhhhhhhhhh"};
        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        auto shp_sym_tab = tree.symbol_table();
        std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

        std::shared_ptr<BitArray> shp_bitstream = make_shared<BitArray>(encode(symbol_table, str));

        REQUIRE(shp_sym_tab != nullptr);
        REQUIRE(shp_bitstream != nullptr);

        //REQUIRE(Hafe::calc_symbol_table_disk_size(symbol_table) == 60);

        auto where = ofstream_open("/home/user/huffman/data/test-5.hafe");
        Hafe hafe{shp_sym_tab, shp_bitstream};
        hafe.write(where);
    }

    { // decompress
        auto where = ifstream_open("/home/user/huffman/data/test-5.hafe");
        Hafe hafe{where};

        auto shp_sym_tab = hafe.symbol_table();
        const auto &symbol_table = *shp_sym_tab;

        REQUIRE(shp_sym_tab != nullptr);

        for (unsigned i = 0; i < symbol_table.size(); ++i) {

            const auto &bit_v = symbol_table[i];
            if (bit_v.empty()) continue;

            const std::string &bit_str{bit_v};
            printf(std::isprint(i) ? "'%c'  | %s\n" : "%#02x | %s\n", i, bit_str.c_str()); // this is so bad.. but it's just 4 dbg :)
        }

        auto shp_bitstream = hafe.bitstream();
        REQUIRE(shp_bitstream != nullptr);

        auto str = decode(symbol_table, *shp_bitstream);
        REQUIRE(str == "abccdddeeeeeffffffffggggggggggggghhhhhhhhhhhhhhhhhhhhh");
    }


    
    { // compress
        std::string str{"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbccccccccccccddddddddddddddddddddddddeeeeeeeeefffff"};
        Histogram freq{(uint8_t *)str.data(), str.length()};

        auto tree = HuffmanTree(freq);
        auto shp_sym_tab = tree.symbol_table();
        std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

        std::shared_ptr<BitArray> shp_bitstream = make_shared<BitArray>(encode(symbol_table, str));

        REQUIRE(shp_sym_tab != nullptr);
        REQUIRE(shp_bitstream != nullptr);

        //REQUIRE(Hafe::calc_symbol_table_disk_size(symbol_table) == 60);

        auto where = ofstream_open("/home/user/huffman/data/test-6.hafe");
        Hafe hafe{shp_sym_tab, shp_bitstream};
        hafe.write(where);
    }

    { // decompress
        auto where = ifstream_open("/home/user/huffman/data/test-6.hafe");
        Hafe hafe{where};

        auto shp_sym_tab = hafe.symbol_table();
        const auto &symbol_table = *shp_sym_tab;

        REQUIRE(shp_sym_tab != nullptr);

        for (unsigned i = 0; i < symbol_table.size(); ++i) {

            const auto &bit_v = symbol_table[i];
            if (bit_v.empty()) continue;

            const std::string &bit_str{bit_v};
            printf(std::isprint(i) ? "'%c'  | %s\n" : "%#02x | %s\n", i, bit_str.c_str()); // this is so bad.. but it's just 4 dbg :)
        }

        auto shp_bitstream = hafe.bitstream();
        REQUIRE(shp_bitstream != nullptr);

        auto str = decode(symbol_table, *shp_bitstream);
        REQUIRE(str == "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbccccccccccccddddddddddddddddddddddddeeeeeeeeefffff");
    }

}