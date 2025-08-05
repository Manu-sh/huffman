#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <iostream>

#include <bitarray/BitArray.hpp>
#include <InverseSymbolTable.hpp>

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


std::string decode(const std::vector<BitArray> &symbol_table, const BitArray &encoded) {

    InverseSymbolTable huffman_codes{symbol_table};

    std::ostringstream decoded;
    BitArray token(1000 * 8 * 1);

    for (uint64_t i = 0, len = encoded.bit_length(); i < len; ++i) {
        token.push_back(encoded[i]);

        if (auto p = huffman_codes.find(token)) {
            decoded.put(*p);
            token.clear();
        }
    }

    return decoded.str();
}
