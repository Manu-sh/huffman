#pragma once

#include <vector>
#include <string>
#include <forward_list>

#include <bitarray/BitArray.hpp>
#include <HuffmanNode.hpp>


// TODO: disequazione di kraft mc millan
// uint8_t bit_required = ceil(log2(pq.size()));
// cout << "bit necessari / profondità albero: " << bit_required  << endl;
// BitArray bvct{bit_required};

// compute a prefix free code using iterative dfs subtree in pre-order
std::shared_ptr<std::vector<BitArray>> build_symbol_table(const HuffmanNode *root) {

    using HuffmanNode::CHILD_LEFT, HuffmanNode::CHILD_RIGHT, std::pair, std::vector; // C++20

    // using std::pair, std::vector;
    // constexpr auto CHILD_LEFT = HuffmanNode::CHILD_LEFT, CHILD_RIGHT = HuffmanNode::CHILD_RIGHT; // C++17

    std::forward_list<pair<const HuffmanNode *, BitArray>> open; // a stack
    auto shp_symbol_table = std::make_shared<vector<BitArray>>(256);
    auto &huffman_code = *shp_symbol_table.get();

    if (root->is_leaf()) {
        huffman_code[root->leaf_data.symbol].push_back(CHILD_LEFT);
        return shp_symbol_table;
    }

    for (open.push_front({root, BitArray{}}); !open.empty();) {

        auto [node, path] = open.front(); // path is a list of edges in binary
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

