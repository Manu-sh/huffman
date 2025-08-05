#pragma once

#include <cstdint>
#include <vector>
#include <utility>

#include <bitarray/BitArray.hpp>

struct InverseSymbolTable final { // faster than unordered_map

    using Bucket = std::vector<std::pair<BitArray, uint8_t>>;
    static const BitArray::HashPrefixCode prefix_code_hash;

    friend std::string decode(const std::vector<BitArray> &symbol_table, const BitArray &encoded);

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


