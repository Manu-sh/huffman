#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <memory>
#include <cstdlib>
#include <algorithm>

#include <HuffmanCode.hpp>
#include <math/math.hpp>

#include <functional>
#include <algorithm>

struct InverseSymbolTable final { // faster than unordered_map

    // if bit_sequence correspond to a valid huffman code the original symbol is returned
    const uint8_t * find(const BitArray &bit_sequence) const noexcept {

        const uint16_t idx = prefix_code_hash(bit_sequence) & (BUCKET_SIZE-1);
        const Bucket &bucket = m_buckets[idx];

#if 0
        // binary search (perform worse)
        auto it = std::lower_bound(bucket.cbegin(), bucket.cend(), bit_sequence, [] (const std::pair<HuffmanCode, uint8_t> &pair, const BitArray &bit_sequence) -> bool {
                return pair.first < bit_sequence;
        });

        return it == bucket.end() || bit_sequence > it->first ? nullptr : &it->second;
#else
        for (uint_fast32_t i = 0, len = bucket.size(); i < len; ++i) {
            const auto &pair = bucket[i];
            if (bit_sequence == pair.first)
                return &pair.second;
            // std::cout << "confronti: " << (i+1) << std::endl;
        }

        return nullptr;
#endif
    }

    private:

        friend class SymbolTable;
        using Bucket = std::vector<std::pair<HuffmanCode, uint8_t>>;
        // TODO: questi sono 40 byte a botta, sicuramente i prefix-code sono lunghi al massimo 256-1
        //  creare una classe/alias prefix-code rendere BitArray Template sulla lunghezza?

        // the maximum length of prefix codes, is 255.
        // the reason is that every prefix free code (length in bits) never exceed the
        // (number of symbols in the alphabet - 1) = 255 bits
        static constexpr uint32_t BUCKET_SIZE = pow2(8); // 15 is the max val
        static_assert(BUCKET_SIZE * sizeof(Bucket) < 786433, "stack smash for sure");

        /*
            prefix-code più lunghi sono meno frequenti, per cui bisogna cercare distribuire le collisioni su elementi lunghi
            anziché usare una distribuzione uniforme come x %256 ovvero x&(256-1)
            bisogna andare su una distribuzione pesata con pesi inversamente proporzionali alla lunghezza del prefix-code
            x % log2(prefix.bit_length), nonostante i molti hash provati questo è quello che si comporta meglio

            Longer prefix codes are less frequent, so we should try to distribute collisions across long elements.
            Instead of using a uniform distribution like x %256 or x&(256-1)
            We should use a weighted distribution with weights inversely proportional to the length of the prefix code.
            x % log2(prefix.bit_length), despite the many hashes we've tried, this one performs best.
        */

        static FORCED(inline) uint64_t prefix_code_hash(const BitArray &prefix_code) {
            return uint8_t(prefix_code.back_byte()) ^ prefix_code.bit_length();
        }

        InverseSymbolTable() = delete;
        InverseSymbolTable(std::shared_ptr<std::vector<HuffmanCode>> shp_symbol_table) {
            const auto &symbol_table = *shp_symbol_table;
            for (uint64_t i = 0, len = symbol_table.size(); i < len; ++i) {
                if (symbol_table[i].empty()) continue;
                this->insert_unique(symbol_table[i], i);
            }

#if 0
            // sort every bucket
            for (uint64_t i = 0; i < BUCKET_SIZE; ++i)
                std::sort(m_buckets[i].begin(), m_buckets[i].end(), [] (const std::pair<HuffmanCode, uint8_t> &kva, const std::pair<HuffmanCode, uint8_t> &kvb) -> bool {
                    return kva.first < kvb.first;
                });
#endif

        }

        void insert_unique(const HuffmanCode &huffman_code, uint8_t symbol) {
            const uint16_t idx = prefix_code_hash(huffman_code) & (BUCKET_SIZE-1);
            m_buckets[idx].emplace_back(huffman_code, symbol);
        }

    protected:
        Bucket m_buckets[BUCKET_SIZE];
};
