#pragma once
extern "C" {
    #include <endian.h>
}

#include <vector>
#include <bitarray/BitArray.hpp>

/*
    | uint8_t | uint16_t | uint8_t[]  |
    ----------|----------|-------------
    | symbol  | length   | prefix code|

    TODO:
        prefix-codes are written with padding bits aligned to the next byte, from byte 0 to byte effective_byte_size()

 */

struct HafeSymbolTable {

    HafeSymbolTable(const std::vector<BitArray> &symbol_table) {
        // htole32(symbol_table[ sym ].bit_length() )
    }

};