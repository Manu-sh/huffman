#pragma once
#include <istream>
#include <cstdint>

#include <hafe_file_format/Hafe.hpp>
#include <ShannonHistogram.hpp>
#include <HuffmanTree.hpp>
#include <BitStream.hpp>


struct Encoder {

    static std::shared_ptr<BitArray> encode(const SymbolTable &st, const std::string &str) {

        auto shp_encoded = std::make_shared<BitArray>(str.length() * 8);
        auto &encoded = *shp_encoded;
        const std::vector<HuffmanCode> &symbol_table = st.borrow();

        for (uint8_t ch : str)
            encoded += symbol_table[ch];

        return shp_encoded;
    }

    static std::shared_ptr<BitArray> encode_fast(const SymbolTable &st, const std::string &str) {

        auto shp_encoded = std::make_shared<BitArray>(str.length() * 8);
        const std::vector<HuffmanCode> &symbol_table = st.borrow();
        auto &encoded = *shp_encoded;

        uint64_t bits = 0;

        for (uint8_t ch : str) {
            const auto &enc = symbol_table[ch];
            /*
            if (!enc.has_padding_bits() && !(bits&7)) {
                memcpy(encoded.data() + (bits/8), enc.bitstream(), enc.effective_byte_size());
                bits += enc.effective_byte_size() * 8;
                continue;
            }*/

            for (uint_fast8_t b = 0, len = enc.bit_length(); b < len; ++b)
                encoded(bits++, enc[b]);
        }

        encoded.bit_length(bits);

        return shp_encoded;
    }



    // NOTE: l'huffman tree è grosso ed è inutile tenerlo in memoria però il tutto è scomponibile per ridurre l'impatto sulla memoria
    explicit Encoder(const std::string &str) {
        this->m_histogram     = ShannonHistogram{(uint8_t *)str.data(), str.length()};
        this->m_symbol_table  = SymbolTable{ HuffmanTree{m_histogram} };
        this->m_bitstream     = BitStream{ encode_fast(m_symbol_table, str) };
        this->m_original_input_length = str.length();
    }

    inline const auto & shannon_histogram() const { return m_histogram;    }
    inline const auto & symbol_table()      const { return m_symbol_table; }
    inline const auto & bitstream()         const { return m_bitstream;    }
    inline double compression_ratio_pct()   const { return ::compression_ratio(m_bitstream.borrow().bit_length(), m_original_input_length * 8); }

    Hafe hafe() const {
        return Hafe(m_symbol_table.share(), m_bitstream.share());
    }

    protected:
        ShannonHistogram m_histogram;
        SymbolTable m_symbol_table;
        BitStream m_bitstream; // encoded bitstream
        uint64_t m_original_input_length;
};