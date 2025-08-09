#pragma once
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <limits>
#include <iterator>
#include <utility>

struct Histogram {

    struct HistogramIterator; // forward declaration

    using value_type      = std::pair<uint8_t, uint32_t>; // Pair<Symbol, Frequency>
    using difference_type = std::ptrdiff_t;
    using pointer         = value_type *;
    using reference       = value_type &;

    using const_iterator  = HistogramIterator;

    struct HistogramIterator {

        // iterator traits
        using iterator_category = std::input_iterator_tag;
        using difference_type   = typename Histogram::difference_type;
        using value_type        = typename Histogram::value_type;
        using pointer           = typename Histogram::pointer;
        using reference         = typename Histogram::reference;

        HistogramIterator() = delete;

        inline explicit HistogramIterator(const Histogram *histogram, uint16_t index = 0)
                : m_histogram{histogram}, m_index{index} {

            // first valid element or std::end()
            while (m_index < 256 && !m_histogram->m_frequency[m_index])
                ++m_index;
        }

        value_type operator*() const {

            if (m_index >= 256) // avoid UB, just die
                throw std::runtime_error{"invalid iterator access"};

            return {m_index, m_histogram->m_frequency[m_index]};
        }

        HistogramIterator & operator++() {
            while (m_index < 256 && !m_histogram->m_frequency[++m_index]); // don't care if std::end() reached it's legal and necessary reach std::end()
            return *this;
        }

        HistogramIterator operator++(int) {
            auto cpy = *this;
            return ++(*this), cpy;
        }

        bool operator==(const HistogramIterator &o) const { return m_index == o.m_index && m_histogram == o.m_histogram; }
        bool operator!=(const HistogramIterator &o) const { return !(*this == o); }

        private:
            const Histogram *m_histogram{};
            uint16_t m_index = -1;
    };

    Histogram() = default;

    explicit Histogram(const uint8_t *data, uint64_t len) {

        if (std::numeric_limits<decltype(len)>::max() == len)
            throw std::runtime_error{"overflow, invalid length value"};

        for (uint64_t i = 0; i < len; ++i)
            m_frequency[ data[i] ]++;
    }

    const_iterator begin() const { return const_iterator(this);      }
    const_iterator   end() const { return const_iterator(this, 256); }

    const_iterator cbegin() const { return std::cbegin(*this); }
    const_iterator   cend() const { return std::cend(*this);   }

    inline auto operator[](uint8_t symbol) const noexcept {
        return m_frequency[ symbol ];
    }

    // TODO: setters
    public:
        uint32_t m_frequency[256] {};
};


static_assert(std::input_iterator<Histogram::HistogramIterator>);
