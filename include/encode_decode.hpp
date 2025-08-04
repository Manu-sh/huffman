#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <ostream>
#include <iostream>

#include <bitarray/BitArray.hpp>

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

// TODO: qua vanno perforza letti i bit
static std::string decode(const std::vector<BitArray> &symbol_table, const BitArray &encoded) {

    std::unordered_map<BitArray, uint8_t, BitArray::HashPrefixCode> dec_sym;
    dec_sym.reserve(256);

    for (unsigned i = 0; i < symbol_table.size(); ++i) {
        if (symbol_table[i].empty()) continue;
        dec_sym[symbol_table[i]] = i;
    }

    std::ostringstream decoded;
    BitArray token(1000 * 8 * 1);

    const auto it_end = dec_sym.end();
    decltype(dec_sym.find(token)) it;

    //for (bool bit : encoded) {
    for (uint64_t i = 0, len = encoded.bit_length(); i < len; ++i) {
        token.push_back(encoded[i]);

        if ((it = dec_sym.find(token)) != it_end) {
            decoded.put(it->second);
            token.clear();
        }
    }

    return decoded.str();
}
