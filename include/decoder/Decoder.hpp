#pragma once
#include <istream>
#include <cstdint>

#include <hafe_file_format/Hafe.hpp>
#include <ShannonHistogram.hpp>
#include <HuffmanTree.hpp>
#include <HuffmanNode.hpp>
#include <BitStream.hpp>
#include <SymbolTable.hpp>
#include <InverseSymbolTable.hpp>


struct Decoder {


#if 1
    // uint64_t steps = huffman_tree_path2symbol(huffman_tree, vbit, vbit_idx, &symbol)
    static uint64_t huffman_tree_path2symbol(const HuffmanTree &tree, const BitArray &encoded,
        uint64_t from_index, uint8_t *symbol) {

        const uint64_t blen = encoded.bit_length();
        assert(from_index < blen);

        const HuffmanNode *cur_node = tree.root();
        uint64_t bit_idx = from_index;

        while (cur_node && !cur_node->is_leaf()) {
            if (bit_idx >= blen) return 0;
            const bool direction = encoded[bit_idx]; // CHILD_LEFT=0, CHILD_RIGHT=1
            cur_node = cur_node->m_child[direction];
            ++bit_idx;
        }

        assert(cur_node);
        assert(cur_node != tree.root());

        *symbol = cur_node->symbol();
        return bit_idx - from_index;
    }
#endif

    static std::shared_ptr<std::string> decode_fast(const HuffmanTree &tree, const BitArray &encoded) {

        std::ostringstream decoded;

        for (uint64_t i = 0; i < encoded.bit_length();) {
            uint8_t symbol;
            uint64_t steps = huffman_tree_path2symbol(tree, encoded, i, &symbol);
            //printf("tree clone -> decoded sym: %c, steps: %zu\n", symbol, steps);

            decoded.put(symbol);
            i += steps;
        }

        return std::make_shared<std::string>( std::move(decoded.str()) );
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

    explicit Decoder(const SymbolTable &symbol_table, const BitStream &encoded)
        : m_symbol_table{symbol_table}, m_bitstream{encoded} {

        //this->m_str  = Decoder::decode(m_symbol_table, m_bitstream.borrow());

        HuffmanTree tree = SymbolTable::to_huffman_tree(symbol_table);
        this->m_str = Decoder::decode_fast(tree, m_bitstream.borrow());
    }

    explicit Decoder(const Hafe &to_decode): Decoder{to_decode.symbol_table(), to_decode.bitstream()} {}

    inline const auto & bitstream()         const { return m_bitstream;    }
    inline const auto & symbol_table()      const { return m_symbol_table; }
    inline const auto & str()               const { return *m_str;                  }
    inline double compression_ratio_pct()   const { return ::compression_ratio(m_bitstream.borrow().bit_length(), m_str->length() * 8); }

    protected:
        SymbolTable m_symbol_table;
        BitStream m_bitstream;
        std::shared_ptr<std::string> m_str;
};
