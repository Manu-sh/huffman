#pragma once

#include <memory>
#include <vector>
#include <queue>

#include <bitarray/BitArray.hpp>
#include <Histogram.hpp>
#include <HuffmanNode.hpp>
#include <huffman.hpp>

struct HuffmanTree {

    explicit HuffmanTree(const Histogram &freq); // standard huffman tree
    explicit HuffmanTree(const Histogram &freq, uint8_t max_bits); // depth-limited huffman tree (for bit-limited prefix-free-codes)

    inline auto symbol_table() const { return ::build_symbol_table(m_root); }

    protected:
        std::vector<std::shared_ptr<HuffmanNode>> gc; // garbage collection
        HuffmanNode *m_root;

};

HuffmanTree::HuffmanTree(const Histogram &freq) {

    static constexpr auto pqueue_cmp = [](const HuffmanNode *a, const HuffmanNode *b) -> bool { return a->freq() > b->freq(); };
    using PQueue = std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, decltype(pqueue_cmp)>;

    for (auto pair : freq)
        gc.push_back(std::make_shared<HuffmanNode>(pair));

    PQueue pq(pqueue_cmp);
    for (auto shp : gc)
        pq.push(shp.get());

    while (pq.size() > 1) {

        HuffmanNode *x = pq.top(); pq.pop();
        if (pq.empty()) break;

        HuffmanNode *y = pq.top(); pq.pop();

        gc.push_back(std::make_shared<HuffmanNode>(x, y));
        pq.push(gc.back().get());
    }

    this->m_root = pq.top();
}
