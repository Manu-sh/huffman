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

/*
La probabilità (assoluta) è definita come:

    N occorrenze / lunghezza insieme
Oppure:
    Frequenza / lunghezza insieme

Es.
    f32 probability = str.count('a') / str.length()
    f32 fractional_bits = log2(1 / probability)

    https://en.wikipedia.org/wiki/Quantities_of_information#Self-information

  in sintesi:
        la quantità di informazione associata a un evento z si misura come il logaritmo negativo della sua probabilità,
        siccome siamo in bit la base del logaritmo diventa n=2,

        per liberarci del meno anziché fare -log2(probability)  facciamo log2(1 / probability)
        ovvero dividiamo 1 per l'argomento del logaritmo così da poter togliere il meno.

        questo valore rappresenta i bit MINIMI necessari per codificare l’evento (secondo Shannon)

    in summary:
        the amount of information associated with an event z is measured as the negative logarithm of its probability,
        since we are working in bits, the base of the logarithm becomes n=2,

        to get rid of the minus sign, instead of doing -log2(probability) we use log2(1 / probability)
        that is, we divide 1 by the argument of the logarithm so we can drop the minus.

        this value represents the MINIMUM number of bits needed to encode the event (according to Shannon)
*/
