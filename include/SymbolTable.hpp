#pragma once
#include <memory>
#include <vector>

#include <HuffmanTree.hpp>
#include <InverseSymbolTable.hpp>
#include <HuffmanCode.hpp>
#include <HuffmanNode.hpp>

#include <string>
#include <forward_list>
#include <cstdio>
#include <cctype>
#include <algorithm>

/*
// TODO: disequazione di kraft mc millan
// uint8_t bit_required = ceil(log2(pq.size()));
// cout << "bit necessari / profondità albero: " << bit_required  << endl;
// BitArray bvct{bit_required};
 */
struct SymbolTable {

    static constexpr uint16_t MAX_SYMBOLS = 256;

    static HuffmanTree to_huffman_tree(const SymbolTable &symbol_table);

    protected:
        static std::shared_ptr<std::vector<HuffmanCode>> build_symbol_table(const HuffmanNode *root);

    public:
        SymbolTable() = default;
        SymbolTable(std::shared_ptr<std::vector<HuffmanCode>> self): m_self{self} {

            if (!self || self->size() != SymbolTable::MAX_SYMBOLS)
                throw std::runtime_error{"Invalid symbol table attached"};
        }

       explicit SymbolTable(const HuffmanTree &tree): SymbolTable{ SymbolTable::build_symbol_table(tree.root()) } {}

        inline auto & mut() const { return *m_self; }
        inline const auto & borrow() const { return *m_self; }
        inline auto share() const { return m_self; }

        void print() const;
        const HuffmanCode & longest() const;

        // const BitArray & operator[](sym) const {}

        InverseSymbolTable inverse_symbol_table() const {
            return InverseSymbolTable{m_self};
        }

    protected:
        std::shared_ptr<std::vector<HuffmanCode>> m_self;

};



// compute a prefix free code using iterative dfs subtree in pre-order
// return an array of 256 elements in which every element is a BitArray
// vector<bits>[256] -> map[sym] = huffman_code
std::shared_ptr<std::vector<HuffmanCode>> SymbolTable::build_symbol_table(const HuffmanNode *root) {

    using HuffmanNode::CHILD_LEFT, HuffmanNode::CHILD_RIGHT, std::pair, std::vector; // C++20

    std::forward_list<pair<const HuffmanNode *, HuffmanCode>> open; // a stack
    auto shp_symbol_table = std::make_shared<vector<HuffmanCode>>(SymbolTable::MAX_SYMBOLS);
    auto &huffman_code = *shp_symbol_table.get();

    if (root->is_leaf()) {
        huffman_code[root->m_symbol].push_back(CHILD_LEFT);
        return shp_symbol_table;
    }

    for (open.push_front({root, HuffmanCode{}}); !open.empty();) {

        auto [node, path] = open.front(); // path is a list of edges in binary (left=0, right=1)
        open.pop_front();

        if (node->is_leaf()) {
            huffman_code[node->m_symbol] = path;
            continue;
        }

        if (node->m_child[CHILD_RIGHT]) {
            open.push_front({node->m_child[CHILD_RIGHT], path}); // aggiungi il nodo alla pila con il relativo percorso fatto per raggiungerlo
            open.front().second.push_back(CHILD_RIGHT);
        }

        if (node->m_child[CHILD_LEFT]) {
            open.push_front({node->m_child[CHILD_LEFT], path});
            open.front().second.push_back(CHILD_LEFT);
        }

    }

    return shp_symbol_table;
}


void SymbolTable::print() const {

    const auto &symbol_table = *m_self;
    for (unsigned i = 0; i < symbol_table.size(); ++i) {

        const auto &bit_v = symbol_table[i];
        if (bit_v.empty()) continue;

        const std::string &bit_str{bit_v};
        printf(std::isprint(i) ? "'%c'  | %s\n" : "%#02x | %s\n", i, bit_str.c_str()); // this is so bad.. but it's just 4 dbg :)
    }

}


// return the longest huffman code or an empty one
const HuffmanCode & SymbolTable::longest() const {

    const auto &symbol_table = *m_self;
    auto it = std::max_element(symbol_table.begin(), symbol_table.end(), [] (const HuffmanCode &a, const HuffmanCode &b) -> bool {
        return a.bit_length() < b.bit_length();
    });

    return it == symbol_table.end() ? *symbol_table.begin() : *it;
}





#if 1
 HuffmanTree SymbolTable::to_huffman_tree(const SymbolTable &symbol_table) {

     HuffmanTree tree;

     tree.gc.push_back(std::make_shared<HuffmanNode>(0, 0));
     tree.m_root = tree.gc.back().get();

    for (uint_fast16_t symbol = 0; symbol < SymbolTable::MAX_SYMBOLS; ++symbol) {

        const HuffmanCode &path = (*symbol_table.m_self)[symbol];
        if (path.is_invalid())
            continue;

        // mkpath() TODO: FUZZY TEST
        HuffmanNode *cur_node = (HuffmanNode *)tree.root();
        for (uint16_t i = 0; i < path.bit_length(); ++i) {

            const bool direction = path[i]; // CHILD_LEFT=0, CHILD_RIGHT=1

            // follow the direction if the node already exist
            if (cur_node->m_child[direction]) {
                cur_node = cur_node->m_child[direction];
                continue;
            }

            // otherwise place a new node
            tree.gc.push_back(std::make_shared<HuffmanNode>(0, 0));
            cur_node = cur_node->m_child[direction] = tree.gc.back().get();
        }

        // at the end of the loop cur_node is pointing to the root or another leaf, every leaf has the symbol
        assert(cur_node);
        assert(cur_node->is_leaf());

        cur_node->m_symbol = symbol;
        cur_node->m_freq   = 1; // a dummy value, we don't have any information about the frequencies stored in the original treee
    }

    return tree;
}
#endif
