#include <cstdint>

#include <iostream>
#include <cctype>
#include <cassert>

#include <encode_decode.hpp>
#include <HuffmanTree.hpp>
#include <Histogram.hpp>
#include <bitarray/BitArray.hpp>

#include <hafe_file_format/HafeHeader.hpp>
#include <hafe_file_format/HafeSymbolTable.hpp>

#include <cstdio>
#include <vector>
#include <cmath>


/*

per la stringa "BPPRRRRRRBPGRGPR" è giusto come huffman code

'B'  | 110
'G'  | 111
'P'  | 10
'R'  | 0


'a'  | 1111110
'b'  | 1111111
'c'  | 111110
'd'  | 11110
'e'  | 1110
'f'  | 110
'g'  | 10
'h'  | 0

*/

// TODO: entropia di un file

inline double probability(uint64_t frequency, uint64_t length) noexcept {
    assert(length > 0);
    return double(frequency) / length;
}

inline double self_information(double probability) noexcept {
    assert(probability > 0);
    return log2(1 / probability);
}

using namespace std;

int main() {

    //auto str = file_content("../data/divina_commedia.txt");
    //auto str = file_content("../data/lorem_ipsum.txt");
    //auto str = std::string("il mio angolo di cielo e un triangolo di pelo");
    auto str = std::string("BPPRRRRRRBPGRGPR");
    Histogram freq{(uint8_t *)str.data(), str.length()};

    // TODO: questo calcola la first order entropy che è approssimativa perchè non tiene conto delle dipendenze tra i simboli
    //  quindi va bene con huffman che un modello a simboli indipendenti
    double weighted_average = 0;
    for (auto [sym, freq] : freq) {
        cout << "s: " << sym << " f: " << freq << " p: " << probability(freq, str.length()) << '\n';
        auto prob = probability(freq, str.length());
        weighted_average += prob * self_information( prob );
    }

    double avg_bit_per_symbol = weighted_average;
    double total_bits = weighted_average * str.length();

    cout << "avg bit per symbol " << avg_bit_per_symbol << endl;
    cout << "the file require at least bits: " << total_bits << " (" << uint64_t(ceil(total_bits / 8)) << " bytes)" << endl;

    //return 0;

    auto tree = HuffmanTree(freq);
    auto shp_sym_tab = tree.symbol_table();
    std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

    for (unsigned i = 0; i < symbol_table.size(); ++i) {

        const auto &bit_v = symbol_table[i];
        if (bit_v.empty()) continue;

        const std::string &bit_str{bit_v};
        printf(std::isprint(i) ? "'%c'  | %s\n" : "%#02x | %s\n", i, bit_str.c_str()); // this is so bad.. but it's just 4 dbg :)
    }


    BitArray encoded = encode(symbol_table, str);
    //cout << encoded.str() << endl;

    auto decoded = decode(symbol_table, encoded);
    //cout << decoded.length() << endl;
    //cout << decoded << endl;
    assert(decoded == str);

    cout << encoded.bit_length() << '/' << (decoded.length() * 8) << " bits" << endl;
    cout << 100. * encoded.bit_length() / (decoded.length() * 8) << '%' << endl;


}
