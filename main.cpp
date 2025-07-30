#include <cstdint>

#include <iostream>
#include <cctype>
#include <cassert>

#include <encode_decode.hpp>
#include <HuffmanTree.hpp>
#include <Histogram.hpp>
#include <bitarray/BitArray.hpp>

#include <cstdio>
#include <vector>
#include <cmath>
#include <fstream>
#include <filesystem>

static std::string file_content(std::string_view fname) {

    static const auto &ifstream_open = [](const char *filename) -> std::ifstream {
        using std::literals::string_literals::operator""s, std::ios_base;
        std::ifstream fpnm;
        fpnm.exceptions(ios_base::badbit);
        fpnm.open(filename, ios_base::in|ios_base::binary);
        if (!fpnm) throw std::invalid_argument{"Unable to open file: "s + filename};
        return fpnm;
    };

    std::ifstream is = ifstream_open(fname.data());
    const std::size_t size = std::filesystem::file_size(fname);
    std::string buffer(size, '\0');  // alloca stringa con spazio già riservato
    is.read(&buffer[0], size);     // lettura diretta
    return buffer;
}


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

// TODO: accettare un istream in decode
// TODO: entropia di un file

using namespace std;

int main() {

    auto str = file_content("../data/divina_commedia.txt");
    //auto str = file_content("../data/lorem_ipsum.txt");
    Histogram freq{(uint8_t *)str.data(), str.length()};

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
