#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <istream>
#include <map>

#include <bitarray/BitArray.hpp>

BitArray encode(const std::vector<BitArray> &symbol_table, const std::string &str) {

    BitArray encoded;
    for (uint8_t ch : str)
        encoded += symbol_table[ch];

    return encoded;
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

// TODO: versione che usa input stream
#if 0
std::string decode(const std::vector<BitArray> &symbol_table, std::istream &bitstream) {

    std::map<std::string, uint8_t> dec_sym;
    for (unsigned i = 0; i < symbol_table.size(); ++i) {
        if (symbol_table[i].empty()) continue;
        dec_sym[symbol_table[i].str()] = i;
    }

    std::ostringstream decoded;
    BitArray token;

    while (bitstream) {

        // TODO: eventualmente aggiungere la possibilità in BitArray di inserire un blocco arbitratio di bit
        //    evitare di leggere un unico byte per volta
        BitArray8 b8;
        bitstream.read((char *)&b8, 1);

        for (int i = 0; i < 8; ++i)
            token.push_back(b8[i]);

        if (auto it = dec_sym.find(token); it != dec_sym.end()) {
            decoded.put(it->second);
            token.clear();
        }
    }

    return decoded.str();
}
#endif