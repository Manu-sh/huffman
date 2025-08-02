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

        // TODO: non è detto che le eccezioni siano abilitate sullo stream che leggo
        using std::ios_base;
        os.exceptions(ios_base::badbit|ios_base::failbit|ios_base::eofbit);

        // write the symbol table size
        uint32_t symbol_table_bsize = htole32(calc_symbol_table_disk_size(map));
        os.write((char *)&symbol_table_bsize, sizeof(uint32_t));

        static const auto &write_entry = [] (std::ostream &os, uint8_t sym, const BitArray &huffman_code) -> void {

            uint16_t length = htole16((uint16_t)huffman_code.bit_length());

            os.put(sym); // symbol
            os.write((char *)&length, sizeof(uint16_t)); // length
            os.write((char *)huffman_code.bitstream(), huffman_code.effective_byte_size()); // prefix code
        };

        for (unsigned sym = 0; sym < 256; ++sym) {
            const auto &huffman_code = map[sym];
            if (huffman_code.empty()) continue;
            write_entry(os, sym, huffman_code);
        }

    }

};


struct HafeSymbolTableDiskReader: protected HafeSymbolTable {

    using HafeSymbolTable::HafeSymbolTable;

    // | symbol  | bit_length(code) | prefix code |
    void read(std::istream &is) {

        // TODO: non è detto che le eccezioni siano abilitate sullo stream che leggo
        using std::ios_base;
        is.exceptions(ios_base::badbit|ios_base::failbit|ios_base::eofbit);

        // read the symbol table size (empty rows aren't stored on disk but they must be accessible later)
        uint32_t symbol_table_bsize;
        if (!is.read((char *)&symbol_table_bsize, sizeof(uint32_t)))
            throw std::runtime_error{"ooops read symbol table entry"};

        symbol_table_bsize = le32toh(symbol_table_bsize);

        // return 0 on failure otherwise the bytes read
        static const auto &read_next_entry = [] (std::vector<BitArray> &map, std::istream &is) -> std::streamsize {

            std::streamsize b_read = 0;

            uint8_t sym;         // symbol
            uint16_t bit_length; // bitstream length in bits without padding

            if (!is.read((char *)&sym, sizeof(uint8_t))) // get() doesn't update gcount()
                return 0;

            b_read += is.gcount();

            if (!is.read((char *)&bit_length, sizeof(uint16_t)))
                return 0;

            b_read += is.gcount();

            // we need this to know at which byte stop reading the bit-stream
            uint16_t byte_length = BitArray::bytes_required(
                    bit_length = le16toh(bit_length)
            );

            // the same value CANNOT be obtained by: BitArray{reserve_bit_length}.effective_byte_size()
            std::vector<BitArray8> bit_stream(byte_length);
            if (!is.read((char *)bit_stream.data(), byte_length)) // load the prefix code (including padding bits)
                return 0;

            map[sym] = BitArray{std::move(bit_stream), bit_length}; // swap the internal buffer (and cut at bit_length bits)
            return b_read += is.gcount();
        };

        for (uint64_t byte_read = 0; byte_read != symbol_table_bsize;) { // i don't like much do-while
            if (!(byte_read = read_next_entry(m_symbol_table, is)))
                throw std::runtime_error{"ooops read symbol table entry"};
        }

    }

    std::vector<BitArray> m_symbol_table = std::vector<BitArray>(256);
};