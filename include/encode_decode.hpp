#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <ostream>
#include <iostream>

#include <bitarray/BitArray.hpp>

//cout << encoded.bit_length() << '/' << (decoded.length() * 8) << " bits" << endl;
//cout << 100. * encoded.bit_length() / (decoded.length() * 8) << '%' << endl;

static std::ostream & print_compression_stats(const BitArray &compressed, const std::string &uncompressed) {
    std::cout << compressed.bit_length() << '/' << (uncompressed.length() * 8) << " bits" << '\n';
    return std::cout << ::compression_rateo(compressed.bit_length(), uncompressed.length() * 8) << '%' << std::endl;
}

// TODO: fornire versioni stream?
static BitArray encode(const std::vector<BitArray> &symbol_table, const std::string &str) {

    BitArray encoded;
    for (uint8_t ch : str)
        encoded += symbol_table[ch];

    //return std::move(encoded); // check this later
    return encoded; // check this later
}

static std::string decode(const std::vector<BitArray> &symbol_table, const BitArray &encoded) {

    std::unordered_map<std::string, uint8_t> dec_sym;
    dec_sym.reserve(256);

    for (unsigned i = 0; i < symbol_table.size(); ++i) {
        if (symbol_table[i].empty()) continue;
        dec_sym[symbol_table[i].str()] = i;
    }

    std::ostringstream decoded;
    BitArray token(1000 * 8 * 1);
    for (bool bit : encoded) {
        token.push_back(bit);

        if (auto it = dec_sym.find(token); it != dec_sym.end()) {
            decoded.put(it->second);
            token.clear();
        }
    }

    return decoded.str();
}
