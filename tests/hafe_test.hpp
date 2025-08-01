#pragma once
#include <doctest/doctest.h>

#include <hafe_file_format/HafeSymbolTable.hpp>

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <iostream>
#include <string>
#include <cstdint>
#include <memory>

#include <ctime>
#include <cstdlib>

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

using namespace std;

TEST_CASE("testing HafeSymbolTableDiskWriter::calc_symbol_table_disk_size()") {

    { // single row test
        auto shp_fake_symtable = std::make_shared<vector<BitArray>>(256);
        auto &fake_symtable = *shp_fake_symtable;

        REQUIRE(HafeSymbolTableDiskWriter::calc_symbol_table_disk_size(fake_symtable) == 0);

        srand(time(0));

        uint8_t sym = rand() % 256;
        auto &row = fake_symtable[sym];

        row.push_back(1); // 1° bit

        REQUIRE(HafeSymbolTableDiskWriter::calc_symbol_table_disk_size(fake_symtable) ==
            sizeof(uint8_t) + sizeof(uint16_t) + row.effective_byte_size()
        );

        REQUIRE(HafeSymbolTableDiskWriter::calc_symbol_table_disk_size(fake_symtable) ==
            1+2+1
        );

        // 7 bit
        row.push_back(1).push_back(1).push_back(1);
        row.push_back(1).push_back(1).push_back(1).push_back(1);

        REQUIRE(HafeSymbolTableDiskWriter::calc_symbol_table_disk_size(fake_symtable) ==
            sizeof(uint8_t) + sizeof(uint16_t) + row.effective_byte_size()
        );

        REQUIRE(HafeSymbolTableDiskWriter::calc_symbol_table_disk_size(fake_symtable) ==
            1+2+1 // 4
        );

        row.push_back(1); // 9° bit, now is larger the row require 2 byte

        REQUIRE(HafeSymbolTableDiskWriter::calc_symbol_table_disk_size(fake_symtable) ==
            sizeof(uint8_t) + sizeof(uint16_t) + row.effective_byte_size()
        );

        REQUIRE(HafeSymbolTableDiskWriter::calc_symbol_table_disk_size(fake_symtable) == 5);
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

            REQUIRE(HafeSymbolTableDiskWriter::calc_symbol_table_disk_size(fake_symtable) ==
                (sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t)) * strlen("BGPR") // size of Rows x Column
            );

        }

}
