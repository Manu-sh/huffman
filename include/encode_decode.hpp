#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <map>

#include <bitarray/BitArray.hpp>


// TODO: fornire versioni stream?
BitArray encode(const std::vector<BitArray> &symbol_table, const std::string &str) {

    BitArray encoded;
    for (uint8_t ch : str)
        encoded += symbol_table[ch];

    //return std::move(encoded); // check this later
    return encoded; // check this later
}

std::string decode(const std::vector<BitArray> &symbol_table, const BitArray &encoded) {

    std::map<std::string, uint8_t> dec_sym;
    for (unsigned i = 0; i < symbol_table.size(); ++i) {
        if (symbol_table[i].empty()) continue;
        dec_sym[symbol_table[i].str()] = i;
    }

    std::ostringstream decoded;
    BitArray token;
    for (bool bit : encoded) {
        token.push_back(bit);

        if (auto it = dec_sym.find(token); it != dec_sym.end()) {
            decoded.put(it->second);
            token.clear();
        }
    }

    return decoded.str();
}
