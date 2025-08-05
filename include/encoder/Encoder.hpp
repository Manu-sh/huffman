#pragma once
#include <istream>
#include <cstdint>

#include <hafe_file_format/Hafe.hpp>
#include <ShannonHistogram.hpp>
#include <HuffmanTree.hpp>
#include <BitStream.hpp>

/*
static std::ostream & print_compression_stats(const BitArray &compressed, const std::string &uncompressed) {
    std::cout << compressed.bit_length() << '/' << (uncompressed.length() * 8) << " bits" << '\n';
    return std::cout << ::compression_rateo(compressed.bit_length(), uncompressed.length() * 8) << '%' << std::endl;
}
*/
struct Encoder {

    static std::shared_ptr<BitArray> encode(const std::vector<BitArray> &symbol_table, const std::string &str) {

        auto shp_encoded = std::make_shared<BitArray>();
        auto &encoded = *shp_encoded;

        for (uint8_t ch : str)
            encoded += symbol_table[ch];

        return shp_encoded;
    }

    // TODO: l'huffman tree è grosso ed è inutile tenerlo in memoria però il tutto è scomponibile per ridurre l'impatto sulla memoria
    explicit Encoder(const std::string &str) {
        this->m_histogram     = ShannonHistogram{(uint8_t *)str.data(), str.length()};
        this->m_symbol_table  = SymbolTable{ HuffmanTree{m_histogram} };
        this->m_bitstream     = BitStream{ encode(m_symbol_table.borrow(), str) };
    }

    inline const auto & shannon_histogram() const { return m_histogram;    }
    inline const auto & symbol_table()      const { return m_symbol_table; }
    inline const auto & bitstream()         const { return m_bitstream;    }

    Hafe hafe() const {
        return Hafe(m_symbol_table.share(), m_bitstream.share());
    }

    protected:
        ShannonHistogram m_histogram;
        SymbolTable m_symbol_table;
        BitStream m_bitstream{nullptr}; // encoded bitstream
};