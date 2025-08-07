#pragma once
#include <istream>
#include <cstdint>

#include <hafe_file_format/Hafe.hpp>
#include <ShannonHistogram.hpp>
#include <HuffmanTree.hpp>
#include <BitStream.hpp>
#include <SymbolTable.hpp>
#include <InverseSymbolTable.hpp>


struct Decoder {

    static std::shared_ptr<std::string> decode(const SymbolTable &symbol_table, const BitArray &encoded) {

        const InverseSymbolTable &huffman_codes = symbol_table.inverse_symbol_table();

        std::ostringstream decoded;
        BitArray token(1000 * 8 * 1);

        for (uint64_t i = 0, len = encoded.bit_length(); i < len; ++i) {

            assert(token.bit_length() < 48); // TODO:
            token.push_back(encoded[i]);

            if (auto p = huffman_codes.find(token)) {
                decoded.put(*p);
                token.clear();
            }
        }

        return std::make_shared<std::string>( std::move(decoded.str()) );
    }

    explicit Decoder(const SymbolTable &symbol_table, const BitStream &encoded)
        : m_symbol_table{symbol_table}, m_bitstream{encoded} {

        this->m_str  = Decoder::decode(m_symbol_table, m_bitstream.borrow());
    }

    explicit Decoder(const Hafe &to_decode): Decoder{to_decode.symbol_table(), to_decode.bitstream()} {}

    inline const auto & bitstream()         const { return m_bitstream.borrow();    }
    inline const auto & symbol_table()      const { return m_symbol_table.borrow(); }
    inline const auto & str()               const { return *m_str;                  }
    inline double compression_ratio_pct()   const { return ::compression_ratio(m_bitstream.borrow().bit_length(), m_str->length() * 8); }

    inline const auto & shannon_histogram() const {
        static const ShannonHistogram histogram{m_symbol_table.borrow(), m_str->length()};
        return histogram;
    }

    protected:
        SymbolTable m_symbol_table;
        BitStream m_bitstream;
        std::shared_ptr<std::string> m_str;
};