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

        cout << a.str() << endl;
        REQUIRE(a.bit_length() == 8);
        REQUIRE(a.bit_capacity() == 8);
        REQUIRE(a.padding_bits() == 0);
        REQUIRE(a.has_padding_bits() == false);
        REQUIRE(b.bit_length() == 8);
        REQUIRE(b.bit_capacity() == 8);
        REQUIRE(b.padding_bits() == 0);
        REQUIRE(b.has_padding_bits() == false);

        REQUIRE((uint8_t)a.back_byte() == 0b111);
        REQUIRE(a.back_byte_without_padding() == 0b111);
        REQUIRE((uint8_t)a.back_byte() == a.back_byte_without_padding());

        REQUIRE((uint8_t)b.back_byte() == 0b111);
        REQUIRE(b.back_byte_without_padding() == 0b111);
        REQUIRE((uint8_t)b.back_byte() == b.back_byte_without_padding());
        REQUIRE((uint8_t)a.back_byte() == (uint8_t)b.back_byte());
        REQUIRE(a.back_byte_without_padding() == b.back_byte_without_padding());

        REQUIRE(a == b);

    }

    {
        HuffmanCode a = HuffmanCode::from<uint8_t>(0b111);
        //HuffmanCode b = HuffmanCode::from<uint8_t>(0b1110'0000);
        HuffmanCode b = HuffmanCode::from<uint8_t>(0b111);
        REQUIRE(a.str() == "00000111");
        REQUIRE(a.str() == b.str());

        // TODO:
        // REQUIRE(HuffmanCode::from("00000111").str() == "00000111");


    }



}
