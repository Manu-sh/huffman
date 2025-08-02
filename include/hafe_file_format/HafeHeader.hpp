#pragma once

extern "C" {
    #include <endian.h>
}

// Huffman Archive Format Example .hafe (all integers in little endian)
struct __attribute__((__packed__)) HafeHeader {

    HafeHeader() = delete;
    HafeHeader(uint32_t symbol_table_size): m_symbol_table_bsize{htole32(symbol_table_size)} {}

    uint32_t symbol_table_bsize() const noexcept { return le32toh(m_symbol_table_bsize); }
    HafeHeader & symbol_table_bsize(uint32_t bsize) noexcept { return m_symbol_table_bsize = htole32(bsize), *this; }

    protected:
        uint8_t m_magic[4] = { 0x4d, 0x61, 0x6e, 0x75 };
        [[maybe_unused]] uint8_t m_reserved[16];
        uint32_t m_symbol_table_bsize;
};