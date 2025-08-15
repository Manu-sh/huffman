#include <cstdint>
#include <iostream>
#include <cassert>

#include <HuffmanTree.hpp>
#include <Histogram.hpp>
#include <ShannonHistogram.hpp>
#include <SymbolTable.hpp>
#include <bitarray/BitArray.hpp>

#include <hafe_file_format/Hafe.hpp>

#include <encoder/Encoder.hpp>
#include <decoder/Decoder.hpp>


using namespace std;

int main(int argc, [[maybe_unused]] char *argv[]) {

    enum  {
        COMPRESS = 1,
        DECOMPRESS
    };

    auto mode = argc > 1 ? DECOMPRESS : COMPRESS;

    if (mode == COMPRESS) { // compress a binary file

        auto str = stream_content(cin);
        Encoder encoder{str};

        // const auto &freq  = encoder.shannon_histogram();
        //auto symbol_table = encoder.symbol_table();
        //assert(symbol_table.longest().bit_length() < HuffmanCode::MAX_LENGTH);

        Hafe hafe = encoder.hafe();
        hafe.write(cout);
        return 0;
    }


    Hafe hafe{std::cin};
    Decoder decoder{hafe};

    //const auto &symbol_table = decoder.symbol_table();
    //assert(symbol_table.longest().bit_length() < HuffmanCode::MAX_LENGTH);
    cout.write((char *)decoder.str().data(), decoder.str().length());

}
