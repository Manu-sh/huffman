#include <cstdint>

#include <iostream>
#include <cctype>
#include <cassert>

#include <encode_decode.hpp>
#include <HuffmanTree.hpp>
#include <Histogram.hpp>
#include <ShannonHistogram.hpp>
#include <bitarray/BitArray.hpp>

#include <hafe_file_format/Hafe.hpp>

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

using namespace std;

int main() {

    // creare una class X <: Histogram contenente anche le stat implicitamente convertibile a Histogram
    // TODO: creare una class SymbolTable <: std::vector<BitArray> contenente anche le stat e renderla implicitamente convertibile a std::vector<BitArray>
    // TODO: aggiungere le stat di compressione in Hafe (?)

    //auto str = file_content("../data/divina_commedia.txt");
    //auto str = file_content("../data/lorem_ipsum.txt");
    auto str = std::string("il mio angolo di cielo e un triangolo di pelo");
    //auto str = std::string("BPPRRRRRRBPGRGPR");
    //Histogram freq{(uint8_t *)str.data(), str.length()};

    ShannonHistogram freq{(uint8_t *)str.data(), str.length()};
    for (auto [sym, f] : freq) {
        freq.dump_entry(sym);
    }
    cout << "avg bit per symbol " << freq.avg_bit_per_symbol() << endl;
    cout << "the file require at least bits: " << freq.total_bits() << " (" << freq.total_bytes() << " bytes)" << endl;

    auto tree = HuffmanTree(freq);
    auto shp_sym_tab = tree.symbol_table();
    std::vector<BitArray> &symbol_table = *shp_sym_tab.get();

    print_symbol_table(symbol_table);

    BitArray encoded = encode(symbol_table, str);
    //cout << encoded.str() << endl;

    auto decoded = decode(symbol_table, encoded);
    assert(decoded == str);

    print_compression_stats(encoded, decoded);

}
