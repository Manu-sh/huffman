#pragma once
#include <cstdint>

extern "C" {
    #include <endian.h>
}

template <typename T> T htobe(T host_order);
template <> uint16_t htobe(uint16_t host_16bits) { return htobe16(host_16bits); }
template <> uint32_t htobe(uint32_t host_32bits) { return htobe32(host_32bits); }
template <> uint64_t htobe(uint64_t host_64bits) { return htobe64(host_64bits); }


template <typename T> T htole(T host_order);
template <> uint16_t htole(uint16_t host_16bits) { return htole16(host_16bits); }
template <> uint32_t htole(uint32_t host_32bits) { return htole32(host_32bits); }
template <> uint64_t htole(uint64_t host_64bits) { return htole64(host_64bits); }


template <typename T> T betoh(T big_endian_order);
template <> uint16_t betoh(uint16_t big_endian_16bits) { return be16toh(big_endian_16bits); }
template <> uint32_t betoh(uint32_t big_endian_32bits) { return be32toh(big_endian_32bits); }
template <> uint64_t betoh(uint64_t big_endian_64bits) { return be64toh(big_endian_64bits); }


template <typename T> T letoh(T little_endian_order);
template <> uint16_t letoh(uint16_t little_endian_16bits) { return le16toh(little_endian_16bits); }
template <> uint32_t letoh(uint32_t little_endian_32bits) { return le32toh(little_endian_32bits); }
template <> uint64_t letoh(uint64_t little_endian_64bits) { return le64toh(little_endian_64bits); }
