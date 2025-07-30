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
    //explicit HuffmanTree(const Histogram &freq, uint8_t max_bits); // depth-limited huffman tree (for bit-limited prefix-free-codes)

    inline auto symbol_table() const { return ::build_symbol_table(m_root); }

    protected:
        std::vector<std::shared_ptr<HuffmanNode>> gc; // garbage collection
        HuffmanNode *m_root;

};

HuffmanTree::HuffmanTree(const Histogram &histogram) {

    /*
    huffman(c)
        n = |c|
        q = c
        for i = 1 to n - 1
            allocate a new node z
            z.left  = x = extract_min(q)
            z.right = y = extract_min(q)
            z.freq  = x.freq + y.freq
            insert(q, z)
        return extract_min(q) // return the root tree
    */

    static constexpr auto pqueue_cmp = [](const HuffmanNode *a, const HuffmanNode *b) -> bool { return a->freq() > b->freq(); };
    using PQueue = std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, decltype(pqueue_cmp)>;

    for (auto [symbol, frequency] : histogram)
        gc.push_back(std::make_shared<HuffmanNode>(symbol, frequency));

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
