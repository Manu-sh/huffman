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
        FORCED(inline) uint8_t take_few(uint8_t to_extract) const {
            assert(to_extract < 8);
            return data >> (8 - to_extract);
        }

    private:
        uint8_t data;
};

static_assert(sizeof(BitArray8) == 1, "BitArray8 must be exactly 1 byte");