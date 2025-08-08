#pragma once
#include <cstdint>
#include <cctype>
#include <cassert>
#include <sstream>
#include <memory>
#include <vector>

#include <HuffmanNode.hpp>

struct HuffmanNode final {

    friend class SymbolTable;

    enum: uint8_t {
        CHILD_LEFT,
        CHILD_RIGHT,
        CHILD_LEN
    };

    inline explicit HuffmanNode(uint8_t symbol, uint32_t frequency) noexcept: m_child{} { // construct an huffman node as leaf
        leaf_data.symbol = symbol;
        leaf_data.freq   = frequency;
    }

    // NOTE: huffman ha problemi di overflow, dal momento che ogni nodo superiore costituisce la somma di altri 2 se 2 foglie fossero con frequenza altissima es. 2**32
    //     i loro genitori su nella gerarchia possono mandare in overflow un uint64

    inline explicit HuffmanNode(HuffmanNode *left, HuffmanNode* right): m_child{left,right} { // construct an huffman node as subtree having n1 and n2 has child
        assert(left && right);

        uint64_t fl = left->freq();
        uint64_t fr = right->freq();

        if (fl >= std::numeric_limits<uint32_t>::max() || fr >= std::numeric_limits<uint32_t>::max())
            throw std::runtime_error{"overflow detected"};

        if (fl+fr >= std::numeric_limits<uint32_t>::max())
            throw std::runtime_error{"overflow detected"};

        freq_sum = fl + fr; // F = n1.freq + n2.freq
        //freq_sum = left->freq() + right->freq(); // F = n1.freq + n2.freq
    }

    inline bool  is_leaf() const noexcept { return m_child[CHILD_LEFT] == m_child[CHILD_RIGHT]; } // nullptr == nullptr
    inline uint64_t freq() const noexcept { return is_leaf() ? leaf_data.freq : freq_sum; }

    void print_subtree(const HuffmanNode *node, int cur_depth = 0) const {

        if (node == nullptr) return;

        std::string tabs(cur_depth++, ' ');
        std::cout << tabs << "=> " << node->freq() << '\n';

        for (const auto child : {node->m_child[CHILD_LEFT], node->m_child[CHILD_RIGHT]})
            print_subtree(child, cur_depth);

    }



    std::string name() const {

        if (!this->is_leaf())
            return std::to_string(this->freq());

        std::ostringstream os;
        if (std::isalnum(this->leaf_data.symbol))
            os << char(this->leaf_data.symbol);
        else
            os << (void*)(long)(this->leaf_data.symbol); // force 2hex conv

        os << ':' << this->freq();
        return os.str();
    }


    operator std::string() const {

        if (this->is_leaf())
            return this->name();

        std::ostringstream os;
        os << "[ " << this->freq() << " ] " << this << '\n'
            << "  left:  "  << this->m_child[CHILD_LEFT]
            << "  right:  " << this->m_child[CHILD_RIGHT]
            << '\n';

        return os.str();
    }

    protected:
        // le foglie avranno certamente frequenza uint32 mentre gli altri nodi non avranno certamente symboli ma avranno frequenze uint64
        HuffmanNode *m_child[CHILD_LEN];

        // you don't need to store frequencies infos into a node since they are used only during the tree building phase,
        // i do for debugging purpose and for accessing in O(1) during the tree-building
        union {
            uint64_t freq_sum;
            struct {
                uint32_t freq;
                uint8_t symbol;
            } leaf_data;
        };

};
