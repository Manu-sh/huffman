#pragma once
#include <doctest/doctest.h>

#include <bitarray/BitArray.hpp>
#include <bitarray/BitArray8.hpp>

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include <iostream>
#include <string>
#include <cstdint>
#include <memory>
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
        REQUIRE(a.padding_bits() == 0);

        a.clear();

        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 8);
        REQUIRE(a.effective_byte_size() == 0);
        REQUIRE(a.padding_bits() == 0);
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
        REQUIRE(a.padding_bits() == 0);
    }

    {
        BitArray a{9};
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 16);

        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);
        REQUIRE(a.padding_bits() == 0);
    }

    {
        BitArray a{16};
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 16);

        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);
        REQUIRE(a.padding_bits() == 0);
    }

    {
        BitArray a{17};
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.bit_capacity() == 24);

        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);
        REQUIRE(a.padding_bits() == 0);
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
            REQUIRE(a.padding_bits() == 8-i);
        }

        // realloc test, inserting the 8th bit
        a.push_back(1);
        REQUIRE(!a.empty());
        REQUIRE(a.last_bit_idx() == 7);
        REQUIRE(a.bit_length() == 8);
        REQUIRE(a.last_element_byte_idx() == 0);

        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 0);


        // 9 bit
        a.push_back(1);
        REQUIRE(!a.empty());

        REQUIRE(a.last_bit_idx() == 8);
        REQUIRE(a.bit_length() == 9);
        REQUIRE(a.last_element_byte_idx() == 1);
        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(a.effective_byte_size() == 2);
        REQUIRE(a.padding_bits() == 7);

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
        REQUIRE(a.padding_bits() == 0);

        a.push_back(1); // PUSH 1
        REQUIRE(a[0] == 1);
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 1);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.bit_capacity() == 8);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 7);

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
        REQUIRE(a.padding_bits() == 7);

        // start remove every bit inserted
        a.pop_back(); // remove 9th bit
        REQUIRE(a.last_bit_idx() == 7);
        REQUIRE(a.bit_length() == 8);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 0);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 6);
        REQUIRE(a.bit_length() == 7);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 1);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 5);
        REQUIRE(a.bit_length() == 6);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 2);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 4);
        REQUIRE(a.bit_length() == 5);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 3);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 3);
        REQUIRE(a.bit_length() == 4);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 4);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 2);
        REQUIRE(a.bit_length() == 3);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 5);

        a.pop_back();
        REQUIRE(a.last_bit_idx() == 1);
        REQUIRE(a.bit_length() == 2);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 6);

        a.pop_back();
        REQUIRE(!a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 1);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 1);
        REQUIRE(a.padding_bits() == 7);

        a.pop_back(); // remove the last bit
        REQUIRE(a.empty());
        REQUIRE(a.last_bit_idx() == 0);
        REQUIRE(a.bit_length() == 0);
        REQUIRE(a.last_element_byte_idx() == 0);
        REQUIRE(a.effective_byte_size() == 0);
        REQUIRE(a.padding_bits() == 0);
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


TEST_CASE("testing concat bitarray::+=operator(const BitArray&)") {
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



TEST_CASE("testing explicit BitArray(BitArray8 *vct, uint64_t byte_length, uint64_t bit_length)") {

    {

        auto byte_len = 3;
        auto shp_bit_a8 = std::make_shared<BitArray8[]>(3); // 24 bit
        BitArray8 *vct8 = shp_bit_a8.get();

        // set 24 bit to 1
        for (int i = 0; i < byte_len; ++i) {
            BitArray8 &el = vct8[i];
            for (int b = 0; b < 8; ++b)
                el(b, 1);
        }

        { // read all 24 bits
            BitArray dst(vct8, byte_len, 24);

            REQUIRE(dst.effective_byte_size() == 3);
            REQUIRE(dst.bit_capacity() == 24);
            REQUIRE(dst.bit_length() == 24);
            REQUIRE(dst.last_bit_idx() == 23);

            for (int i = 0; i < dst.bit_length(); ++i)
                REQUIRE(dst[i] == 1);
        }


        { // read only 3 bits
            BitArray dst(vct8, byte_len, 3);

            REQUIRE(dst.effective_byte_size() == 1);
            REQUIRE(dst.bit_capacity() == 8);
            REQUIRE(dst.bit_length() == 3);
            REQUIRE(dst.last_bit_idx() == 2);

            for (int i = 0; i < dst.bit_length(); ++i)
                REQUIRE(dst[i] == 1);
        }


    }

}

TEST_CASE("testing explicit BitArray(vector<BitArray8>&&, uint64_t bits)") {

    {
        std::vector<BitArray8> bit_stream(3); // 24 bit
        for (int i = 0; i < bit_stream.size(); ++i) {
            BitArray8 &el = bit_stream[i];
            for (int b = 0; b < 8; ++b)
                el(b, 1);
        }

        BitArray dst{std::move(bit_stream), 24}; // move ownership

        REQUIRE(dst.effective_byte_size() == 3);
        REQUIRE(dst.bit_capacity() == 24);
        REQUIRE(dst.bit_length() == 24);
        REQUIRE(dst.last_bit_idx() == 23);

        for (int i = 0; i < dst.bit_length(); ++i)
            REQUIRE(dst[i] == 1);

        // once moved you cannot use anymore
    }

    {
        std::vector<BitArray8> bit_stream(3); // 24 bit
        for (int i = 0; i < bit_stream.size(); ++i) {
            BitArray8 &el = bit_stream[i];
            for (int b = 0; b < 8; ++b)
                el(b, 1);
        }

        BitArray dst{std::move(bit_stream), 3}; // move ownership

        REQUIRE(dst.effective_byte_size() == 1);
        REQUIRE(dst.bit_capacity() == 24); // i may change this behavior later to be dst.bit_capacity() == 8
        REQUIRE(dst.bit_length() == 3);
        REQUIRE(dst.last_bit_idx() == 2);

        for (int i = 0; i < dst.bit_length(); ++i)
            REQUIRE(dst[i] == 1);

        // once moved you cannot use anymore
    }

    { // no 0 length
        std::vector<BitArray8> bit_stream(0);
        CHECK_THROWS(BitArray(std::move(bit_stream), 3));
    }


    {
        std::vector<BitArray8> bit_stream(1);
        BitArray dst(std::move(bit_stream), 3);

        REQUIRE(dst.effective_byte_size() == 1);
        REQUIRE(dst.bit_capacity() == 8);
        REQUIRE(dst.bit_length() == 3);
        REQUIRE(dst.last_bit_idx() == 2);
    }

    { // no overflow
        std::vector<BitArray8> bit_stream(1);
        CHECK_THROWS(BitArray(std::move(bit_stream), 9));
    }

}


TEST_CASE("testing advance compare") {

    {
        BitArray bit_src(8);
        BitArray bit_dst(8);

        bit_src.push_back(1);
        bit_dst.push_back(1);

        BitArray8 &dst_back_byte = bit_dst.back_byte();
        BitArray8 &src_back_byte = bit_src.back_byte();
        REQUIRE(src_back_byte[0] == 1);

        // simulate a different padding at the end between bit_dst & bit_src
        src_back_byte(1, 1);
        dst_back_byte(1, 0);

        REQUIRE(bit_src.bit_length() == 1);
        REQUIRE(bit_dst.bit_length() == 1);
        REQUIRE(dst_back_byte != src_back_byte);
        REQUIRE(bit_src == bit_dst);
    }


    {

        for (int i = 0; i < 128; ++i) {

            BitArray bit_src(i+1);
            BitArray bit_dst(i+1);
            //BitArray calimero(i+1);

            for (int k = 1; k <= i+1; ++k) {
                bit_src.push_back(1);
                bit_dst.push_back(1);
                //calimero.push_back(0);
            }

            BitArray8 &dst_back_byte = bit_dst.back_byte();
            BitArray8 &src_back_byte = bit_src.back_byte();
            REQUIRE(src_back_byte[0] == dst_back_byte[0]);
            REQUIRE(src_back_byte[1] == dst_back_byte[1]);

            // simulate a different padding at the end between bit_dst & bit_src
            if ((i+1)%8 != 0) {
                src_back_byte((i+1)%8, 1);
                dst_back_byte((i+1)%8, 0);
            }

            REQUIRE(bit_src.bit_length() == i+1);
            REQUIRE(bit_dst.bit_length() == i+1);
            
            REQUIRE(bit_src.back() == 1);
            REQUIRE(bit_dst.back() == 1);
            REQUIRE(bit_src.back() == bit_dst.back());

            if ((i+1)%8 != 0) {
                REQUIRE(dst_back_byte != src_back_byte);
            } else {
                REQUIRE(dst_back_byte == src_back_byte);
            }

            //REQUIRE(bit_src == bit_dst);

            if (bit_src != bit_dst) {

                //cout << i << endl;
                bool x = bit_src == bit_dst;
                if ((i+1)%8 != 0) {
                    REQUIRE(x == true);
                } else {
                    REQUIRE(x == false);
                }
            }
        }


    }


}



TEST_CASE("testing BitArray::from()") {


    {
        BitArray a; a.push_back(1).push_back(1).push_back(1);
        BitArray b; b.push_back(1).push_back(1).push_back(1);
        REQUIRE(a == b);
    }


    {
        BitArray a = BitArray::from<uint8_t>(0b111);
        BitArray b = BitArray::from<uint8_t>(0b111);
        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(b.bit_capacity() == 16);
        REQUIRE(a == b);
    }


    {
        BitArray a = BitArray::from(uint8_t(0b111));
        BitArray b = BitArray::from(uint8_t(0b111));
        REQUIRE(a.bit_capacity() == 16);
        REQUIRE(b.bit_capacity() == 16);
        REQUIRE(a == b);
    }

    {
        BitArray a = BitArray::from(uint16_t(0b111));
        BitArray b = BitArray::from(uint16_t(0b111));
        REQUIRE(a.bit_capacity() == 8 * (sizeof(uint16_t) +  sizeof(uint8_t)));
        REQUIRE(a.bit_capacity() == b.bit_capacity());
        REQUIRE(a == b);
    }


    {
        BitArray a = BitArray::from(uint32_t(0b111));
        BitArray b = BitArray::from(uint32_t(0b111));
        REQUIRE(a.bit_capacity() == 8 * (sizeof(uint32_t) + sizeof(uint8_t)));
        REQUIRE(a.bit_capacity() == b.bit_capacity());
        REQUIRE(a == b);
    }

}
