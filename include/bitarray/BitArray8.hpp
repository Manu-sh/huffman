#pragma once
#include <cstdint>
#include <cassert>
#include <common.hpp>

struct __attribute__((__packed__)) BitArray8 {

    //friend class BitArray;

    private:
        FORCED(inline) BitArray8 & set_bit(uint8_t i) {
            data |= (1 << (7-i));
            return *this;
        }

        FORCED(inline) BitArray8 & clear_bit(uint8_t i) {
            data &= ~(1 << (7-i));
            return *this;
        }

    public:
        // explicit BitArray8(uint8_t byte): data{byte} {}

        FORCED(inline) bool operator[](uint8_t i) const {
            assert(i < 8);
            return (data >> (7 - i)) & 1;
        }

        FORCED(inline) void operator()(uint8_t i, bool value) {
            return (void)(value ? this->set_bit(i) : this->clear_bit(i));
        }

        FORCED(inline) explicit operator uint8_t() const noexcept {
            return data;
        }

        FORCED(inline) bool operator==(const BitArray8 &) const = default;

        // helper to cut away the padding bits: data >> (8 - to_extract));
        // this is like the ''.substr(0, bit_length) method but for binary,
        // valid ranges for bit_length is 1-8
        FORCED(inline) uint8_t take_few(uint8_t bit_length) const {
            assert(bit_length < 8);
            return data >> (8 - bit_length);
        }

    private:
        uint8_t data;
};

static_assert(sizeof(BitArray8) == 1, "BitArray8 must be exactly 1 byte");