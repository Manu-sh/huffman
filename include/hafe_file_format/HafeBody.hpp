#pragma once
extern "C" {
    #include <endian.h>
}

#include <vector>
#include <memory>
#include <stdexcept>

#include <istream>
#include <ostream>
#include <ios>

#include <bitarray/BitArray.hpp>
#include <bitarray/BitArray8.hpp>

/*

`uint8_t rest` stream_size % 2**8, a compact way to express bitstream length since can be very long and it contains garbage at the end (padding bits).
bitstream..

*/

struct HafeBody {
    HafeBody() = delete;
};

