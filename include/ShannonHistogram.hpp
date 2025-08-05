#pragma once
#include <Histogram.hpp>
#include <math/math.hpp>

#include <cstdint>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <iostream>

// this class only store "useless" stats that can be useful for debug etc, to compute huffman all you need is Histogram
struct ShannonHistogram: public Histogram {

    struct shannon_entry_t {

        shannon_entry_t & calc(uint32_t frequency, uint64_t dataset_length) {
            this->probability      = shannon_probability(frequency, dataset_length);
            this->self_information = shannon_self_information( this->probability );
            return *this;
        }

        double probability;
        double self_information;
    };

    ShannonHistogram() = default;

    explicit ShannonHistogram(const uint8_t *data, uint64_t len): Histogram{data, len}, m_original_dataset_length{len} {

        // This calculates the first-order entropy, which is approximate because it doesn't take into account the dependencies between symbols.
        // So it's fine with Huffman's model, which is a model with independent symbols.
        m_avg_bit_per_symbol = 0;
        for (auto [sym, freq] : ((Histogram *)this)[0]) { // iterate over non-zero frequencies
            m_map[sym].calc(freq, m_original_dataset_length);
            m_avg_bit_per_symbol += m_map[sym].probability * m_map[sym].self_information;
        }

        m_total_bits = m_avg_bit_per_symbol * m_original_dataset_length;
    }

    inline double avg_bit_per_symbol() const noexcept { return m_avg_bit_per_symbol; }                  // average in how many bit you can encode each symbol
    inline double         total_bits() const noexcept { return m_total_bits; }                          // minimum number of bits required to encode the dataset
    inline uint64_t      total_bytes() const noexcept { return uint64_t(std::ceil(m_total_bits / 8)); } // minimum number of bytes required to encode the dataset

    // TODO: eventualmente usare una std::multimap e definire una relazione d'ordine in base alle frequenze oppure ordinare e basta
    void dump_entry(uint8_t symbol) const {

        using std::cout, std::endl;
        const auto default_precision{std::cout.precision()};

        Histogram &hs = ((Histogram *)this)[0];

        cout << std::setprecision(4);
        if (std::isprint(symbol))
            cout << "'" << char(symbol) << "'";
        else
            cout << ((void *)(long)(symbol));

        cout << std::right << std::setfill(' ') << std::setw(7);

        cout << " | f: " << std::left << std::setfill(' ') << std::setw(5) << hs[symbol] <<
                " | p: " << std::left << std::setfill(' ') << std::setw(4+3) << m_map[symbol].probability <<
                " | i: " << m_map[symbol].self_information << '\n';

        std::cout << std::setprecision(default_precision); // restore defaults
    }

    protected:
        shannon_entry_t m_map[256] {};
        uint64_t m_original_dataset_length;
        double m_avg_bit_per_symbol;
        double m_total_bits; // minimum number of bits required to encode the dataset
};