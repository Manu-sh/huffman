#pragma once
#include <cstdint>
#include <vector>
#include <utility>

#include <bitarray/BitArray.hpp>

struct InverseSymbolTable final { // faster than unordered_map

    const uint8_t * find(const BitArray &huffman_code) const noexcept {

        const uint16_t idx = prefix_code_hash(huffman_code) ^ 255;
        const Bucket &bucket = buckets[idx];

        for (uint32_t i = 0, len = bucket.size(); i < len; ++i) {
            const auto &pair = bucket[i];
            if (huffman_code == pair.first)
                return &pair.second;
        }

        return nullptr;
    }

    private:
        struct HashPrefixCode final {

            // prefix-code più lunghi sono meno frequenti, per cui bisogna cercare distribuire le collisioni su elementi lunghi
            // anzichè usare una distribuzione uniforme come (x+1)%256 ovvero x^(256-1)
            // bisogna andare su una distribuzione pesata con pesi inversamente proporzionali alla lunghezza del prefix-code
            // x ^ log2(prefix.bit_length) ovvero -> (x+1) % log2(prefix.bit_length)

            FORCED(inline) std::size_t operator()(const BitArray &prefix_code) const noexcept {
                //return std::hash<uint8_t>()( (uint8_t)prefix_code.back_byte() ) ^ std::hash<uint64_t>()( prefix_code.bit_length() );
                //return (uint8_t)prefix_code.back_byte() ^ prefix_code.bit_length();

                //return (uint8_t)prefix_code.back_byte() ^ (uint64_t)log2(prefix_code.bit_length());
                //return (uint8_t(prefix_code.back_byte())+1) % floor_log2(prefix_code.bit_length());
                //return (uint8_t)prefix_code.back_byte() ^ prefix_code.bit_length();

                //return ::hash_murmur_oaat64((void *)&prefix_code.back_byte(), 1) ^ prefix_code.bit_length();
                //return ::hash_murmur_oaat64((void *)&prefix_code.back_byte(), 1) % 256;

                // return ::hash_murmur_oaat64((void *)prefix_code.m_vct.data(), prefix_code.effective_byte_size()) ^ prefix_code.bit_length();
                //return ::hash_murmur_oaat64((void *)prefix_code.m_vct.data(), prefix_code.effective_byte_size()) ^ 255;
                //return ::hash_murmur_oaat64((void *)&prefix_code.back_byte(), 1) ^ 255;

                //return (uint8_t)prefix_code.back_byte() ^ 255;

                //return (uint8_t)prefix_code.back_byte() ^ prefix_code.bit_length();
                //return (uint8_t)prefix_code.back_byte() % (floor_log2(prefix_code.bit_length() + 1) + 1);
                //return (uint8_t)prefix_code.back_byte() ^ (floor_log2(prefix_code.bit_length() + 1) + 1);
                //return  ::hash_murmur_oaat64((void *)prefix_code.m_vct.data(), prefix_code.effective_byte_size()) % (floor_log2(prefix_code.bit_length() + 1) + 1);
                // return  ::hash_murmur_oaat64((void *)prefix_code.m_vct.data(), prefix_code.effective_byte_size()) ^ (floor_log2(prefix_code.bit_length() + 1) + 1);

                return uint8_t(prefix_code.back_byte()) ^ prefix_code.bit_length();
            }
        };

        using Bucket = std::vector<std::pair<BitArray, uint8_t>>;
        static const HashPrefixCode prefix_code_hash;

        friend class SymbolTable;
        friend class Decoder;
        friend std::string decode(const std::vector<BitArray> &symbol_table, const BitArray &encoded);

        InverseSymbolTable() = delete;
        InverseSymbolTable(const std::vector<BitArray> &symbol_table) {

            for (uint64_t i = 0, len = symbol_table.size(); i < len; ++i) {
                if (symbol_table[i].empty()) continue;
                this->insert_unique(symbol_table[i], i);
            }
        }

        void insert_unique(const BitArray &huffman_code, uint8_t symbol) {
            const uint16_t idx = prefix_code_hash(huffman_code) ^ 255;
            buckets[idx].emplace_back(huffman_code, symbol);
        }

    protected:
        std::vector<Bucket> buckets{256};

};