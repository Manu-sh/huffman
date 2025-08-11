#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <queue>

#include <Histogram.hpp>
#include <HuffmanNode.hpp>

struct HuffmanTree {

    friend class SymbolTable;

    HuffmanTree() = default;
    explicit HuffmanTree(const Histogram &freq); // standard huffman tree
    //explicit HuffmanTree(const Histogram &freq, uint8_t max_bits); // depth-limited huffman tree (for bit-limited prefix-free-codes)

    inline const HuffmanNode * root() const { return m_root; }
    inline auto height() { // this value is used in testing this is why i use recursion (for now)

        if (m_height > 0) return m_height;

        // lazy on m_height initialization
        HuffmanNode::recursive_dfs([this] ([[maybe_unused]] const HuffmanNode *node, uint32_t cur_depth) {
            if (cur_depth > this->m_height)
                this->m_height = cur_depth;
        }, m_root);

        return m_height;
    }

    void print() const { // 4 debugging

        static const auto cb = [] (const HuffmanNode *node, uint32_t cur_depth) {
            std::cout << cur_depth << std::endl;
            std::string tabs(cur_depth++, ' ');
            std::cout << tabs << "=> " << node->freq() << '\n';
        };

        HuffmanNode::recursive_dfs(cb, m_root);
    }

    protected:
        std::vector<std::shared_ptr<HuffmanNode>> gc; // garbage collection
        HuffmanNode *m_root;
        uint32_t m_height = 0;
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

    for (auto [symbol, frequency] : histogram) {
        assert(frequency > 0);
        gc.push_back(std::make_shared<HuffmanNode>(symbol, frequency));
    }

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
