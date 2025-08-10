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


DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <iostream>
#include <string>
#include <cstdint>
#include <memory>

#include <ctime>
#include <cstdlib>

extern "C" {
    #include <endian.h>
}
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

using namespace std;

TEST_CASE("testing HuffmanCode") {

    {
        // TODO: trasformare a big endian poi vedere il da farsi
        HuffmanCode a = HuffmanCode::from<uint8_t>(0b111);
        HuffmanCode b = HuffmanCode::from<uint8_t>(0b0000'0111);
        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(b.bit_capacity() == 16);
        REQUIRE(a == b);
        REQUIRE(a.back_byte_without_padding() == 0);
    }



}
