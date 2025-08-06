#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <memory>
#include <cstdlib>

#include <bitarray/BitArray.hpp>

static constexpr uint32_t pow2(uint8_t exp) {
    return 2 << (exp - 1);
}

struct InverseSymbolTable final { // faster than unordered_map

    const uint8_t * find(const BitArray &huffman_code) const noexcept {

        const uint16_t idx = prefix_code_hash(huffman_code) & (BUCKET_SIZE-1);
        const Bucket &bucket = m_buckets[idx];

        for (uint32_t i = 0, len = bucket.size(); i < len; ++i) {
            const auto &pair = bucket[i];
            if (huffman_code == pair.first)
                return &pair.second;
        }

        return nullptr;
    }

    private:

        static constexpr uint32_t BUCKET_SIZE = pow2(15);
        //static_assert(BUCKET_SIZE == 256, "ops");

        friend class SymbolTable;
        using Bucket = std::vector<std::pair<BitArray, uint8_t>>;

        // prefix-code più lunghi sono meno frequenti, per cui bisogna cercare distribuire le collisioni su elementi lunghi
        // anziché usare una distribuzione uniforme come x %256 ovvero x&(256-1)
        // bisogna andare su una distribuzione pesata con pesi inversamente proporzionali alla lunghezza del prefix-code
        // x % log2(prefix.bit_length), nonostante i molti hash provati questo è quello che si comporta meglio

        static FORCED(inline) uint64_t prefix_code_hash(const BitArray &prefix_code) {
            return uint8_t(prefix_code.back_byte()) ^ prefix_code.bit_length();
            //assert(prefix_code.bit_length());
            //return (BUCKET_SIZE - prefix_code.bit_length()) % BUCKET_SIZE;
            //return prefix_code.bit_length() & (BUCKET_SIZE - 1);
        }

        InverseSymbolTable() = delete;
        InverseSymbolTable(std::shared_ptr<std::vector<BitArray>> shp_symbol_table) {
            const auto &symbol_table = *shp_symbol_table;
            for (uint64_t i = 0, len = symbol_table.size(); i < len; ++i) {
                if (symbol_table[i].empty()) continue;
                this->insert_unique(symbol_table[i], i);
            }

        }

        void insert_unique(const BitArray &huffman_code, uint8_t symbol) {
            const uint16_t idx = prefix_code_hash(huffman_code) & (BUCKET_SIZE-1);
            m_buckets[idx].emplace_back(huffman_code, symbol);
        }

    protected:
        Bucket m_buckets[BUCKET_SIZE];

};
