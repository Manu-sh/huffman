#pragma once
#include <doctest/doctest.h>

#include <bitarray/BitArray.hpp>

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <iostream>
#include <string>
#include <cstdint>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

using namespace std;

TEST_CASE("testing empty bitarray") {

    {
        BitArray a;
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 8);

        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);

        a.clear();

        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 8);
        REQUIRE(a.effective_byte_size() == 0);
    }

}

TEST_CASE("testing bitarray{length}") {

    {
        BitArray a{8};
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 8);

        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);
    }

    {
        BitArray a{9};
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 16);

        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);
    }

    {
        BitArray a{16};
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 16);

        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);
    }

    {
        BitArray a{17};
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 24);

        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);
    }

}


TEST_CASE("testing bitarray::push_back()") {

    { // pushback_test
        BitArray a;

        for (int i = 1; i <= 7; ++i) {  // no realloc test
            a.push_back(1);
            REQUIRE(!a.empty());

            REQUIRE(a.last_bit_idx() == i-1);
            REQUIRE(a.bit_length() == i);

            REQUIRE(a.bit_capacity() == 8);
            REQUIRE(a.last_element_byte_idx() == 0);
            REQUIRE(a.effective_byte_size() == 1);
        }

        // realloc test, inserting the 8th bit
        a.push_back(1);
        REQUIRE(!a.empty());
        REQUIRE(a.last_bit_idx() == 7);
        REQUIRE(a.bit_length() == 8);
        REQUIRE(a.last_element_byte_idx() == 0);

        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(a.effective_byte_size() == 1);

        // 9 bit
        a.push_back(1);
        REQUIRE(!a.empty());

        REQUIRE(a.last_bit_idx() == 8);
        REQUIRE(a.bit_length() == 9);
        REQUIRE(a.last_element_byte_idx() == 1);
        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(a.effective_byte_size() == 2);

        REQUIRE(a[8] == 1);
    }


}

TEST_CASE("testing bitarray::pop_back()") {

    {
        BitArray a;
        a.push_back(0); // PUSH 0
        REQUIRE(a[0] == 0);

        a.pop_back(); // remove
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.bit_capacity() == 8);
        REQUIRE(a.effective_byte_size() == 0);

        a.push_back(1); // PUSH 1
        REQUIRE(a[0] == 1);
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 1);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.bit_capacity() == 8);
        REQUIRE(a.effective_byte_size() == 1);

        a.clear();

        a.push_back(1); // PUSH 8 bit
        a.push_back(1);
        a.push_back(1);
        a.push_back(1);
        a.push_back(1);
        a.push_back(1);
        a.push_back(1);
        a.push_back(1);

        for (int i = 0; i < 8; ++i)
            REQUIRE(a[i] == 1);

        // push 9th bit
        a.push_back(1);
        REQUIRE(a[8] == 1);
        REQUIRE(a.last_bit_idx() == 8);
        REQUIRE(a.bit_length() == 9);
        REQUIRE(a.last_element_byte_idx() == 1);
        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(a.effective_byte_size() == 2);

        // start remove every bit inserted
        a.pop_back(); // remove 9th bit
        REQUIRE(a.last_bit_idx() == 7);
        REQUIRE(a.bit_length() == 8);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 6);
        REQUIRE(a.bit_length() == 7);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 5);
        REQUIRE(a.bit_length() == 6);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 4);
        REQUIRE(a.bit_length() == 5);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);


        a.pop_back();
        REQUIRE(a.last_bit_idx() == 3);
        REQUIRE(a.bit_length() == 4);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 2);
        REQUIRE(a.bit_length() == 3);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 1);
        REQUIRE(a.bit_length() == 2);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);

        a.pop_back();
        REQUIRE(!a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 1);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);

        a.pop_back(); // remove the last bit
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);

    }

    {
        BitArray a{9};
        for (int i = 0; i < 9; ++i)
            a.push_back(1);

        REQUIRE(!a.empty());
        REQUIRE(a.last_bit_idx() == 8);
        REQUIRE(a.bit_length() == 9);
        REQUIRE(a.last_element_byte_idx() == 1);
        REQUIRE(a.effective_byte_size() == 2);
        REQUIRE(a.bit_capacity() == 16);

        // remove all elements except the last one
        for (int i = a.bit_length(); i > 1; --i) {
            REQUIRE(!a.empty());
            REQUIRE(a.last_bit_idx() == (i-1));
            REQUIRE(a.bit_length() == (i-1)+1);

            REQUIRE(a.last_element_byte_idx() == ((i-1) == 8 ? 1 : 0));
            a.pop_back();
        }

        a.pop_back(); // remove the last bit
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);


    }

}


TEST_CASE("testing bitarray::=operator(const BitArray&)") {

    {
        const BitArray src{128};
        BitArray dst;

        REQUIRE(src.bit_capacity() == 128);
        REQUIRE(src.bit_length() == 0);

        REQUIRE(dst.bit_capacity() == 8);
        REQUIRE(dst.bit_length() == 0);

        dst = src;
        REQUIRE(src.bit_capacity() == 128);
        REQUIRE(src.bit_length() == 0);
    }

    {
        BitArray src{128};
        for (int i = 0; i < 33; ++i)
            src.push_back(1);

        BitArray dst;

        REQUIRE(src.bit_capacity() == 128);
        REQUIRE(src.bit_length() == 33);
        REQUIRE(src.last_bit_idx() == 32);

        REQUIRE(dst.bit_capacity() == 8);
        REQUIRE(dst.bit_length() == 0);

        dst = src;
        REQUIRE(dst.bit_capacity() == 128);
        REQUIRE(dst.bit_length() == 33);
        REQUIRE(dst.last_bit_idx() == 32);

        for (int i = 0; i < 33; ++i)
            REQUIRE(dst[i] == src[i]);
    }


}


TEST_CASE("testing bitarray::+=operator(const BitArray&)") {
    {
        BitArray src{128};
        for (int i = 0; i < 33; ++i)
            src.push_back(1);

        BitArray dst;
        dst += src;
        dst += src;

        REQUIRE(dst.bit_capacity() >= 33*2);
        REQUIRE(dst.bit_length() == 33*2);
        REQUIRE(dst.last_bit_idx() == 33*2-1);

        REQUIRE(dst.last_element_byte_idx() == 8);
        REQUIRE(dst.effective_byte_size() == 9);

        for (int i = 0; i < dst.bit_length(); ++i)
            REQUIRE(dst[i] == 1);

    }
}