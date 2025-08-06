#pragma once

extern "C" {
    #include <endian.h>
}

#include <memory>
#include <istream>
#include <ostream>
#include <ios>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring>

#include <bitarray/BitArray.hpp>
#include <bitarray/BitArray8.hpp>
#include <SymbolTable.hpp>
#include <BitStream.hpp>

// Huffman Archive Format Example .hafe (all integers in little endian)
struct Hafe {

    static std::shared_ptr<std::vector<BitArray>> read_symbol_table(std::istream &is, uint32_t symbol_table_bsize);
    static uint32_t calc_symbol_table_disk_size(const SymbolTable &st);
    static void write_symbol_table(std::ostream &os, const SymbolTable &st);

    Hafe() = delete;
    explicit Hafe(std::istream &is) { this->read(is); } // Decode stored Hafe file
    explicit Hafe(const SymbolTable &st, const BitStream &bit_stream) { // Store encoded Hafe File
        memset(m_reserved, 0x1d, sizeof m_reserved); // easy debugging
        this->symbol_table(st).bitstream(bit_stream);
    }

    void write(std::ostream &os) const;
    inline auto symbol_table() const { return m_symbol_table; }
    inline auto bitstream()    const { return m_bit_stream;   }

    private:
        // tecnicamente se modifichi la symbol table dovresti modificare anche il bitstream
        inline Hafe & symbol_table(const SymbolTable &st) {

            assert(st.share());

            this->symbol_table_size(Hafe::calc_symbol_table_disk_size(st));
            m_symbol_table = st;
            return *this;
        }

        inline Hafe & bitstream(const BitStream &bit_stream) {
            assert(bit_stream.share());

            bitstream_bitsz(bit_stream.borrow().bit_length());
            m_bit_stream = bit_stream;
            return *this;
        }

#ifdef DBG_HAFE_BIG_ENDIAN
        inline Hafe & symbol_table_size(uint32_t symbol_table_size) { return m_symbol_table_bsize = htobe32(symbol_table_size), *this; }
        inline Hafe & bitstream_bitsz(uint64_t bit_size) { return m_bitstream_bitsz = htobe64(bit_size), *this; }
        inline uint32_t symbol_table_size() const noexcept { return be32toh(m_symbol_table_bsize); }
        inline   uint64_t bitstream_bitsz() const noexcept { return be64toh(m_bitstream_bitsz);    }
#else
        inline Hafe & symbol_table_size(uint32_t symbol_table_size) { return m_symbol_table_bsize = htole32(symbol_table_size), *this; }
        inline Hafe & bitstream_bitsz(uint64_t bit_size) { return m_bitstream_bitsz = htole64(bit_size), *this; }
        inline uint32_t symbol_table_size() const noexcept { return le32toh(m_symbol_table_bsize); }
        inline   uint64_t bitstream_bitsz() const noexcept { return le64toh(m_bitstream_bitsz);    }
#endif

        void read(std::istream &is);


    // Every integer will be stored in hafe format endian
    private:
        // TOP:
        uint8_t m_magic[4] {};
        [[maybe_unused]] uint8_t m_reserved[16];
        uint32_t m_symbol_table_bsize; // TODO: empty rows aren't stored on disk but they must be accessible later)

        // SymTable
        SymbolTable m_symbol_table;

        // Bottom
        uint64_t m_bitstream_bitsz;
        BitStream m_bit_stream;
};


void Hafe::read(std::istream &is) {

    // read magic
    if (!is.read((char *)m_magic, 4) || memcmp((void *)m_magic, (void *)"\x4d\x61\x6e\x75", 4))
        throw std::runtime_error{"missing or invalid magic number"};


    // read reserved bytes (could be ignored)
    if (!is.read((char *)m_reserved, sizeof m_reserved))
        throw std::runtime_error{"missing reserved bytes"};

    // read symbol_table length
    if (!is.read((char *)&m_symbol_table_bsize, sizeof m_symbol_table_bsize))
        throw std::runtime_error{"missing symbol table length"};

    // read symbol table
    m_symbol_table = read_symbol_table(is, this->symbol_table_size()); // here pass the symbol table size in host order

    // read bitstream size in bits
    if (!is.read((char *)&m_bitstream_bitsz, sizeof m_bitstream_bitsz))
        throw std::runtime_error{"missing bitstream size in bits"};


    // finally read the bitstream
    auto bitstream_bytes = BitArray::bytes_required(this->bitstream_bitsz());
    std::vector<BitArray8> bit_stream{bitstream_bytes}; // allocate enough space

    // read the bitstream
    if (!is.read((char *)bit_stream.data(), bitstream_bytes))
        throw std::runtime_error{"incomplete bitstream"};

    // change the ownership of bit_stream and tell BitArray to use it as internal buffer (avoid realloc copy etc)
    //m_bit_stream = std::make_shared<BitArray>(std::move(bit_stream), this->bitstream_bitsz());
    m_bit_stream = BitStream{ std::make_shared<BitArray>(std::move(bit_stream), this->bitstream_bitsz()) };
}


void Hafe::write(std::ostream &os) const {

    if (!m_symbol_table.share() || !m_bit_stream.share())
        throw std::runtime_error{"incomplete format, pls set symbol table and bitstream before calling Hafe::write()"};

    // write magic
    if (!os.write((char *)"\x4d\x61\x6e\x75", 4))
        throw std::runtime_error{"error writing the magic number"};


    // write space for reserved bytes
    if (!os.write((char *)m_reserved, sizeof m_reserved))
        throw std::runtime_error{"error writing reserved bytes"};

    // write symbol table
    write_symbol_table(os, m_symbol_table);

    // write the bitstream size in bits
    if (!os.write((char *)&m_bitstream_bitsz, sizeof m_bitstream_bitsz))
        throw std::runtime_error{"error writing reserved bytes"};

    if (!os.write((char *)m_bit_stream.borrow().bitstream(), m_bit_stream.borrow().effective_byte_size()))
        throw std::runtime_error{"error writing the bitstream"};
}



// | symbol  | bit_length(code) | prefix code |
std::shared_ptr<std::vector<BitArray>> Hafe::read_symbol_table(std::istream &is, uint32_t symbol_table_bsize) {

    // empty rows aren't stored on disk but they must be accessible later so we cannot use symbol_table_bsize as argument, it must be 256
    auto shp_sym_table = std::make_shared<std::vector<BitArray>>(256);
    auto &symbol_table = *shp_sym_table.get();

    // TODO: non è detto che le eccezioni siano abilitate sullo stream che leggo
    using std::ios_base;
    is.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);

    // return 0 on failure otherwise the bytes read
    static const auto &read_next_entry = [](std::vector <BitArray> &map, std::istream &is) -> std::streamsize {

        std::streamsize b_read = 0;

        uint8_t sym;         // symbol
        uint16_t bit_length; // bitstream length in bits without padding

        if (!is.read((char *)&sym, sizeof(uint8_t))) // get() doesn't update gcount()
            return 0;

        b_read += is.gcount();

        if (!is.read((char *)&bit_length, sizeof(uint16_t)))
            return 0;

        b_read += is.gcount();

        // we need this to know at which byte stop reading the bit-stream
        uint16_t byte_length = BitArray::bytes_required(
#ifdef DBG_HAFE_BIG_ENDIAN
                bit_length = be16toh(bit_length)
#else
                bit_length = le16toh(bit_length)
#endif
        );

        if (byte_length == 0)
            throw std::runtime_error{"refuse to create a symbol table entry with size 0"};

        //std::cout << "sym:" << sym << ":bysz:" << byte_length << std::endl;

        // the same value CANNOT be obtained by: BitArray{reserve_bit_length}.effective_byte_size()
        std::vector <BitArray8> bit_stream(byte_length);
        if (!is.read((char *)bit_stream.data(), byte_length)) // load the prefix code (including padding bits)
            return 0;

        map[sym] = BitArray{std::move(bit_stream), bit_length}; // swap the internal buffer (and cut at bit_length bits)
        return b_read += is.gcount();
    };

    for (uint64_t byte_read = 0; byte_read != symbol_table_bsize;) { // i don't like much do-while
        if (!(byte_read += read_next_entry(symbol_table, is)))
            throw std::runtime_error{"ooops read symbol table entry"};
    }

    return shp_sym_table;
}

uint32_t Hafe::calc_symbol_table_disk_size(const SymbolTable &st) {

    const auto &symbol_table = st.borrow();

    uint32_t bsize = 0;
    for (unsigned sym = 0; sym < 256; ++sym) {
        const auto &huffman_code = symbol_table[sym];
        if (huffman_code.empty()) continue;

        bsize += sizeof(uint8_t);  // symbol
        bsize += sizeof(uint16_t); // length
        bsize += huffman_code.effective_byte_size(); // bitstream size in bytes
    }

    return bsize;
}


void Hafe::write_symbol_table(std::ostream &os, const SymbolTable &st) {

    const auto &symbol_table = st.borrow();

    // TODO: non è detto che le eccezioni siano abilitate sullo stream che leggo
    using std::ios_base;
    os.exceptions(ios_base::badbit|ios_base::failbit|ios_base::eofbit);

    // write the symbol table size
#ifdef DBG_HAFE_BIG_ENDIAN
    uint32_t symbol_table_bsize = htobe32(Hafe::calc_symbol_table_disk_size(symbol_table));
#else
    uint32_t symbol_table_bsize = htole32(Hafe::calc_symbol_table_disk_size(st));
#endif

    if (!os.write((char *)&symbol_table_bsize, sizeof(uint32_t)))
        throw std::runtime_error{"error writing the symbol table size"};

    static const auto &write_entry = [] (std::ostream &os, uint8_t sym, const BitArray &huffman_code) -> bool {

#ifdef DBG_HAFE_BIG_ENDIAN
        uint16_t length = htobe16((uint16_t)huffman_code.bit_length());
#else
        uint16_t length = htole16((uint16_t)huffman_code.bit_length());
#endif

        os.put(sym); // symbol
        os.write((char *)&length, sizeof(uint16_t)); // length
        os.write((char *)huffman_code.bitstream(), huffman_code.effective_byte_size()); // prefix code

        return bool{os};
    };

    for (unsigned sym = 0; sym < 256; ++sym) {
        const auto &huffman_code = symbol_table[sym];
        if (huffman_code.empty()) continue;
        if (!write_entry(os, sym, huffman_code))
            throw std::runtime_error{"write_entry(): error writing the symbol table"};
    }

}