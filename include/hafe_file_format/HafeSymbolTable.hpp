#pragma once
extern "C" {
    #include <endian.h>
}

#include <vector>
#include <memory>
#include <bitarray/BitArray.hpp>

/*
    | uint8_t | uint16_t         | uint8_t[]   |
    ----------|------------------|-------------
    | symbol  | bit_length(code) | prefix code |

    TODO:
        prefix-codes are written with padding bits aligned to the next byte, from byte 0 to byte effective_byte_size()
 */

struct HafeSymbolTable {

    protected:
        HafeSymbolTable(std::shared_ptr<const std::vector<BitArray>> symbol_table) {
            if (symbol_table->size() != 256)
                throw std::runtime_error{"Invalid symbol table size"};

            m_symbol_table = symbol_table;
        }

        // htole16(symbol_table[ sym ].bit_length() )

        std::shared_ptr<const std::vector<BitArray>> m_symbol_table;
};

// TODO: nel caso di una lettura del file da disco sarà sempre little endian, nel caso della scrittura invece no

struct HafeSymbolTableDiskWriter: protected HafeSymbolTable {

    using HafeSymbolTable::HafeSymbolTable;

    static uint32_t calc_symbol_table_disk_size(const std::vector<BitArray> &symbol_table) {

        uint32_t bsize = 0;
        for (unsigned sym = 0; sym < 256; ++sym) {
            const auto &huffman_code = symbol_table[sym];
            if (huffman_code.empty()) continue;

            bsize += sizeof(uint8_t);  // symbol
            bsize += sizeof(uint16_t); // length
            bsize += huffman_code.effective_byte_size(); // bitstream size in bytes
        }

        return bsize;
    }

    void write(std::ostream &os) const {

        const auto &map = *m_symbol_table;

        uint32_t symbol_table_bsize = htole32(calc_symbol_table_disk_size(map));
        os.write((char *)&symbol_table_bsize, sizeof(uint32_t));

        for (unsigned sym = 0; sym < 256; ++sym) {
            const auto &huffman_code = map[sym];
            if (huffman_code.empty()) continue;

            uint16_t length = htole16((uint16_t)huffman_code.bit_length());
            os.put(sym); // symbol
            os.write((char *)&length, sizeof(uint16_t)); // length
            os.write((char *)huffman_code.bitstream(), huffman_code.effective_byte_size()); // prefix code
        }

    }

};


struct HafeSymbolTableDiskReader: protected HafeSymbolTable {
    using HafeSymbolTable::HafeSymbolTable;
};