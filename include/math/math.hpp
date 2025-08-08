#pragma once
#include <cmath>
#include <cstdint>
#include <cassert>

#include <common.hpp>


// il logaritmo in base X di un argomento A si può calcolare
// usando il logaritmo in base 10: log10(X) / log10(A)
static double log_in_base(double base, double argument) {
    return std::log10(argument) / std::log10(base);
}

static constexpr FORCED(inline) uint32_t pow2(uint8_t exp) {
    return 2 << (exp - 1);
}

// ceil_div(x, 8) -> same of (int)ceil(x/8.)
static constexpr FORCED(inline) uint64_t ceil_div(uint64_t num, uint8_t div) noexcept {
    return (num-1) / div + 1;
}

template <const bool use_pct = 1>
static FORCED(inline) double compression_ratio(uint64_t compressed_len, uint64_t uncompressed_len) noexcept {
    static_assert(use_pct == 1, "not implemented");
    assert(compressed_len && uncompressed_len);
    return 100. * compressed_len / uncompressed_len;
}


static FORCED(inline) double shannon_probability(uint64_t huffman_code_length) noexcept {
    // return 1.0 / std::pow<double>(2, huffman_code_length); // division lead precision issues here
    return std::pow<double>(2, -huffman_code_length);
}

static FORCED(inline) double shannon_probability(uint64_t frequency, uint64_t length) noexcept {
    assert(length > 0);
    return double(frequency) / length;
}

static FORCED(inline) double shannon_self_information(double probability) noexcept {
    assert(probability > 0);
    return std::log2(1 / probability); // return std::log2(std::pow<double>(2, -probability));
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


#if 0
// fibonacci tends to produce the worst case scenario in huffman tree(s)
// https://stackoverflow.com/questions/19883086/optimal-huffman-code-for-fibonacci-numbers#19893487
// https://github.com/ahmedss33/Introduction-to-Algorithms-Solutions/blob/master/C16-Greedy-Algorithms/16.3.md

// skipping the zero return every other element of fibonacci until max_times is reached or fibo(n) > max_fibo
template <typename T>
static std::vector<T> fibonacci_gen(uint32_t from, uint32_t to, uint64_t max_fibo = std::numeric_limits<T>::max()) {

    if (max_fibo == std::numeric_limits<uint64_t>::max())
        --max_fibo;

    std::vector<T> numbers{1, 1};

    uint64_t a = 1, b = 1;
    uint64_t cur_fibo = a + b;

    for (uint32_t i = from; i < to; ++i, cur_fibo = a + b) {
        if (cur_fibo >= max_fibo)
            return numbers;

        numbers.push_back(cur_fibo);
        a = b;
        b = cur_fibo;
    }

    return numbers;
}
#endif