#pragma once
#include <cmath>
#include <cstdint>
#include <cassert>

#include <common.hpp>

// ceil_div(x, 8) -> same of (int)ceil(x/8.)
static constexpr FORCED(inline) uint64_t ceil_div(uint64_t num, uint8_t div) noexcept {
    return (num-1) / div + 1;
}

template <const bool use_pct = 1>
static FORCED(inline) double compression_rateo(uint64_t compressed_len, uint64_t uncompressed_len) noexcept {
    static_assert(use_pct == 1, "not implemented");
    assert(compressed_len && uncompressed_len);
    return 100. * compressed_len / uncompressed_len;
}

static FORCED(inline) double shannon_probability(uint64_t frequency, uint64_t length) noexcept {
    assert(length > 0);
    return double(frequency) / length;
}

static FORCED(inline) double shannon_self_information(double probability) noexcept {
    assert(probability > 0);
    return std::log2(1 / probability);
}
