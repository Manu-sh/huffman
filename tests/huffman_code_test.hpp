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
        HuffmanCode a = HuffmanCode::from<uint8_t>(0b111);
        HuffmanCode b = HuffmanCode::from<uint8_t>(0b0000'0111);
        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(b.bit_capacity() == 16);
        REQUIRE(a == b);
        REQUIRE(a.back_byte_without_padding() == 0);
    }

    {
        HuffmanCode a = HuffmanCode::from<uint8_t>(0b111u);
        HuffmanCode b = HuffmanCode::from<uint8_t>(0b1110'0000u);
        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(b.bit_capacity() == 16);
        REQUIRE(a == b);
        REQUIRE(a.back_byte_without_padding() == 0);
    }



}
