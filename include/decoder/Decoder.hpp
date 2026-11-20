#pragma once
#include <istream>
#include <cstdint>
#include <cassert>

#include <hafe_file_format/Hafe.hpp>
#include <ShannonHistogram.hpp>
#include <HuffmanTree.hpp>
#include <HuffmanNode.hpp>
#include <BitStream.hpp>
#include <SymbolTable.hpp>
#include <InverseSymbolTable.hpp>


struct Decoder {

    //
    static uint64_t decode_next_symbol(const HuffmanTree &tree, const BitArray &encoded,
        const uint64_t from_index, uint8_t *symbol) {

        const uint64_t blen = encoded.bit_length();
        const HuffmanNode *cur_node = tree.root();
        uint64_t bit_idx;

        if (from_index >= blen || !cur_node)
            return 0;

        for (bit_idx = from_index; bit_idx < blen; ++bit_idx) {

            if (cur_node->is_leaf()) { // symbol decoded
                *symbol = cur_node->symbol();
                return bit_idx - from_index;
            }

            const bool direction = encoded[bit_idx]; // CHILD_LEFT=0, CHILD_RIGHT=1
            if (!(cur_node = cur_node->m_child[direction]))
                return 0; // invalid path/tree
        }

        *symbol = cur_node->symbol();
        return bit_idx - from_index; //return std::min(bit_idx, blen) - from_index;
    }


    static std::shared_ptr<std::string> decode(const SymbolTable &symbol_table, const BitArray &encoded) {

        const InverseSymbolTable &huffman_codes = symbol_table.inverse_symbol_table();

        std::ostringstream decoded;
        BitArray token(1000 * 8 * 1);

        for (uint64_t i = 0, len = encoded.bit_length(); i < len; ++i) {

            assert(token.bit_length() < HuffmanCode::MAX_LENGTH);
            token.push_back(encoded[i]);

            if (auto p = huffman_codes.find(token)) {
                decoded.put(*p);
                token.clear();
            }
        }

        return std::make_shared<std::string>( std::move(decoded.str()) );
    }


    static std::shared_ptr<std::string> decode(const HuffmanTree &tree, const BitArray &encoded) {

        std::ostringstream decoded;

        for (uint64_t i = 0; i < encoded.bit_length();) {
            uint8_t symbol;
            uint64_t steps = decode_next_symbol(tree, encoded, i, &symbol);
            //printf("tree clone -> decoded sym: %c, steps: %zu\n", symbol, steps);

            decoded.put(symbol);
            i += steps;
        }

        return std::make_shared<std::string>( std::move(decoded.str()) );
    }

    explicit Decoder(const SymbolTable &symbol_table, const BitStream &encoded, bool use_tree)
        : m_symbol_table{symbol_table}, m_bitstream{encoded} {

        if (!use_tree) {
            this->m_str  = Decoder::decode(m_symbol_table, m_bitstream.borrow());
            return;
        }

        const HuffmanTree &tree = symbol_table.to_huffman_tree();
        this->m_str = Decoder::decode(tree, m_bitstream.borrow());
    }

    explicit Decoder(const Hafe &to_decode, bool use_tree): Decoder{to_decode.symbol_table(), to_decode.bitstream(), use_tree} {}

    inline const auto & bitstream()         const { return m_bitstream;    }
    inline const auto & symbol_table()      const { return m_symbol_table; }
    inline const auto & str()               const { return *m_str;                  }
    inline double compression_ratio_pct()   const { return ::compression_ratio(m_bitstream.borrow().bit_length(), m_str->length() * 8); }

    protected:
        SymbolTable m_symbol_table;
        BitStream m_bitstream;
        std::shared_ptr<std::string> m_str;
};
