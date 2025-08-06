#pragma once
#include <memory>
#include <vector>

#include <bitarray/BitArray.hpp>
#include <HuffmanTree.hpp>

#include <string>
#include <forward_list>
#include <cstdio>
#include <cctype>

/*
// TODO: disequazione di kraft mc millan
// uint8_t bit_required = ceil(log2(pq.size()));
// cout << "bit necessari / profondità albero: " << bit_required  << endl;
// BitArray bvct{bit_required};
 */
struct SymbolTable {

    protected:
        static std::shared_ptr<std::vector<BitArray>> build_symbol_table(const HuffmanNode *root);

    public:
        SymbolTable() = default;
        SymbolTable(std::shared_ptr<std::vector<BitArray>> self): m_self{self} {}
        explicit SymbolTable(const HuffmanTree &tree): SymbolTable{ SymbolTable::build_symbol_table(tree.root()) } {}

        inline auto & mut() const { return *m_self; }
        inline const auto & borrow() const { return *m_self; }
        inline auto share() const { return m_self; }

        void print() const;
        // const BitArray & operator[](sym) const {}

        /*
        InverseSymbolTable inverse_symbol_table() const {
            return InverseSymbolTable{*m_self};
        }*/

    protected:
        std::shared_ptr<std::vector<BitArray>> m_self;

};



// compute a prefix free code using iterative dfs subtree in pre-order
// return an array of 256 elements in which every element is a BitArray
// vector<bits>[256] -> map[sym] = huffman_code
std::shared_ptr<std::vector<BitArray>> SymbolTable::build_symbol_table(const HuffmanNode *root) {

    using HuffmanNode::CHILD_LEFT, HuffmanNode::CHILD_RIGHT, std::pair, std::vector; // C++20

    std::forward_list<pair<const HuffmanNode *, BitArray>> open; // a stack
    auto shp_symbol_table = std::make_shared<vector<BitArray>>(256);
    auto &huffman_code = *shp_symbol_table.get();

    if (root->is_leaf()) {
        huffman_code[root->leaf_data.symbol].push_back(CHILD_LEFT);
        return shp_symbol_table;
    }

    for (open.push_front({root, BitArray{}}); !open.empty();) {

        auto [node, path] = open.front(); // path is a list of edges in binary (left=0, right=1)
        open.pop_front();

        if (node->is_leaf()) {
            huffman_code[node->leaf_data.symbol] = path;
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