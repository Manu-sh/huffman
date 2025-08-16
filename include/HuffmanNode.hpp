#pragma once
#include <cstdint>
#include <cctype>
#include <cassert>
#include <memory>
#include <format>

struct HuffmanNode final {

    friend class SymbolTable;

    enum: uint8_t {
        CHILD_LEFT,
        CHILD_RIGHT,
        CHILD_LEN
    };


    static void recursive_dfs(auto callback, const HuffmanNode *node, uint32_t cur_depth = 0) {

        if (node == nullptr) return;

        callback(node, cur_depth++);

        for (const auto child : {node->m_child[CHILD_LEFT], node->m_child[CHILD_RIGHT]})
            recursive_dfs(callback, child, cur_depth);
    }



    inline explicit HuffmanNode(uint8_t symbol, uint32_t frequency) noexcept: m_child{} { // construct an huffman node as leaf
        m_symbol = symbol;
        m_freq   = frequency;
    }

    // NOTE: huffman ha problemi di overflow, dal momento che ogni nodo superiore costituisce la somma di altri 2 se 2 foglie fossero con frequenza altissima es. 2**32
    //     i loro genitori su nella gerarchia possono mandare in overflow un uint64

    inline explicit HuffmanNode(HuffmanNode *left, HuffmanNode *right): m_child{left,right} { // construct an huffman node as subtree having n1 and n2 has child
        assert(left && right);

        const uint64_t &fl = left->m_freq;
        const uint64_t &fr = right->m_freq;

        if (fl >= std::numeric_limits<uint32_t>::max() || fr >= std::numeric_limits<uint32_t>::max())
            throw std::runtime_error{"overflow detected"};

        if (fl+fr >= std::numeric_limits<uint32_t>::max())
            throw std::runtime_error{"overflow detected"};

        m_freq = fl + fr; // F = n1.freq + n2.freq
    }

    inline bool  is_leaf() const noexcept { return m_child[CHILD_LEFT] == m_child[CHILD_RIGHT]; } // nullptr == nullptr
    inline uint64_t freq() const noexcept { return m_freq; }


    std::string name() const {

        if (!this->is_leaf())
            return std::to_string(this->m_freq);

        return std::isalnum(m_symbol) ? std::format("{}:{}", char(m_symbol), m_freq) : std::format("{}:{}", (void*)(long)m_symbol, m_freq); // force 2hex conv
    }


    operator std::string() const {

        if (this->is_leaf())
            return this->name();

        return std::format("[{}] {}\n  left: {}  right: {}\n", m_freq, (void *)this, (void *)m_child[CHILD_LEFT], (void *)m_child[CHILD_RIGHT]);
    }

    protected:
        // le foglie avranno certamente frequenza uint32 mentre gli altri nodi non avranno certamente symboli ma avranno frequenze uint64
        HuffmanNode *m_child[CHILD_LEN];

        // you don't need to store frequencies infos into a node since they are used only during the tree building phase,
        // i do for debugging purpose and for accessing in O(1) during the tree-building
        uint64_t m_freq;
        uint8_t m_symbol;

};
