#pragma once
#include <doctest/doctest.h>

#include <hafe_file_format/Hafe.hpp>
#include <common.hpp>
#include <HuffmanCode.hpp>
#include <HuffmanTree.hpp>
#include <Histogram.hpp>
#include <SymbolTable.hpp>
#include <bitarray/BitArray.hpp>
#include <encoder/Encoder.hpp>
#include <decoder/Decoder.hpp>
#include <endian/endianess.hpp>

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <iostream>
#include <string>
#include <cstdint>
#include <memory>

#include <ctime>
#include <cstdlib>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

using namespace std;

TEST_CASE("testing HuffmanCode") {

    {
        HuffmanCode a = "111"_vbit;
        HuffmanCode b = "00000111"_vbit;

        cout << a.str() << endl;
        REQUIRE(a.bit_length() == 3);
        REQUIRE(a.bit_capacity() == 8);
        REQUIRE(a.padding_bits() == 5);
        REQUIRE(a.has_padding_bits() == true);
        REQUIRE(b.bit_length() == 8);
        REQUIRE(b.bit_capacity() == 16);
        REQUIRE(b.padding_bits() == 0);
        REQUIRE(b.has_padding_bits() == false);
        REQUIRE(b.str() == "00000111");

    }

    {
        HuffmanCode a = "111"_vbit;
        HuffmanCode b = "111"_vbit;
        REQUIRE(a.str() == "111");
        REQUIRE(a.str() == b.str());

    }



}
