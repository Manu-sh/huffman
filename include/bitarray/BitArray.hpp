#pragma once

#include <cstdint>
#include <vector>
#include <sstream>
#include <iterator>
#include <cstddef>
#include <cassert>
#include <cstring>

#include "../common.hpp"
#include "BitArray8.hpp"

// TODO: reserve() and resize() method

// TODO: avere effective_byte_size() sempre ad almeno 1 semplificherebbe molto l'arimentica
//   usare empty per determinare se è il caso di skippare la copia probabilmente va riscritta poi la parte memcpy e aggiornati i test

struct BitArray {

    // es. 9 bit requires 2 byte -> byte_required(9) -> 2
    static inline constexpr auto bytes_required(uint64_t bits) noexcept {
        //return bits == 0 ? 1 : ceil_div(bits, 8);
        return ceil_div(bits, 8);
    }

    public:
        // effective_byte_size() -> return in which byte is stored the latest bit 0 if none
        // so DONT write m_vct[effective_byte_size()-1]
        // the underlying container (vector) can be bigger than the actual byte size
        inline uint64_t effective_byte_size() const noexcept {
            return bit_length() == 0 ? 0 : bytes_required(bit_length());
        }

        // last_element_byte_idx() -> m_vct[ last_element_byte_idx() ] always is valid
        inline uint64_t last_element_byte_idx() const noexcept {

            // m_bit_idx point to the next unused bit
            // if we have 8 bit stored his value is 8 but the last element is stored into this[7]

           return effective_byte_size() == 0 ? 0 : effective_byte_size() - 1;
        }

        FORCED(inline) uint64_t last_bit_idx() const noexcept {
            return m_bit_idx - (m_bit_idx != 0); // same of: m_bit_idx == 0 ? m_bit_idx : m_bit_idx-1
        }

public:

    struct BitArrayIterator; // forward declaration

    using value_type      = bool;
    using difference_type = std::ptrdiff_t;
    using pointer         = value_type *;
    using reference       = value_type &;
    using const_iterator  = BitArrayIterator;


    public:
        explicit BitArray(uint32_t bit_length = 1):
            m_vct(bytes_required(bit_length)),
            m_bit_capacity{bytes_required(bit_length) * 8} // number of bit slots effectively available
        {

        }

        BitArray(const BitArray &o) {
            this->operator=(o);
        }

        BitArray & operator=(const BitArray &o) {

            // i have not time 4 a full implementation
            assert(&o != this);
            assert(&(o.m_vct) != &(this->m_vct));;

            m_vct.resize(o.m_vct.size());
            //memcpy(m_vct.data(), o.m_vct.data(), o.m_vct.size());
            memcpy(m_vct.data(), o.m_vct.data(), o.effective_byte_size());
            m_bit_capacity = o.m_bit_capacity;
            m_bit_idx = o.m_bit_idx;
            return *this;
        }

        BitArray & operator+=(const BitArray &o);

        inline uint64_t bit_length()   const noexcept { return m_bit_idx;         } // bits you have pushed with push function
        inline uint64_t bit_capacity() const noexcept { return m_bit_capacity;    } // in this container bit_capacity()-1 is always accessible, this value is at least 1
        inline     bool empty()        const noexcept { return !m_bit_idx;        }

        inline void clear() {

            m_vct.clear();
            m_vct.resize(1);

            m_bit_idx = 0;
            m_bit_capacity = bytes_required(1) * 8;
        }

        bool operator[](uint64_t i) const {
            assert(i < m_bit_capacity);
            const auto byte_idx = i >> 3; // (i/8)
            //const BitArray8 &bit_a = m_vct.at(byte_idx);
            const BitArray8 &bit_a = m_vct[byte_idx];
            return bit_a[i&7]; // i%8
        }

        void operator()(uint64_t i, bool value) {
            const auto byte_idx = i >> 3; // (i/8)
            //BitArray8 &bit_a = m_vct.at(byte_idx);
            BitArray8 &bit_a = m_vct[byte_idx];
            return (void)bit_a(i&7, value); // i%8
        }

        bool back() const { // if is_empty() calling back() result in UB and is your fault
            assert(!empty());
            return this->operator[](
                    //m_bit_idx - (m_bit_idx != 0) // same of: m_bit_idx == 0 ? m_bit_idx : m_bit_idx-1
                    last_bit_idx()
            );
        }

        void push_back(bool value) {

            if (m_bit_idx >= m_bit_capacity-1) {
                m_vct.push_back({}); // attach a new 8 bit chunk
                m_bit_capacity = m_vct.size() << 3; // size() * 8 //m_bit_length += 8;
            }

            this->operator()(m_bit_idx++, value); // insert the value and point at the next bit
        }

        // TODO: ogni tanto è opportuno fare shrink del vector sottostante se m_vct.size() è molto grande rispetto a this->bit_size() / 8
        void pop_back() {
            assert(!empty());
            --m_bit_idx;
        }


        operator std::string() const {

            std::ostringstream oss;
            for (uint64_t i = 0; i < this->bit_length(); ++i)
                oss << char(this[0][i] + '0');

            return oss.str();
        }

        std::string str() const {
            return std::string{*this};
        }

        /*
        void reserve(uint64_t bit_length) {

            auto byte_length = ceil_div(bit_length, 8);
            if (bit_length <= bit_length())
                return;

            m_vct->reserve( bytes );
            m_bit_capacity = bit_length;
        }*/

        const_iterator begin() const;
        const_iterator   end() const;
        const_iterator cbegin() const;
        const_iterator   cend() const;

        inline const void * bitstream() const { return m_vct.data(); }

        protected:
            std::vector<BitArray8> m_vct; // used as memory block
            uint64_t m_bit_idx{}; // index where insert the next value, when array created = 0
            uint64_t m_bit_capacity{};
};

struct BitArray::BitArrayIterator {

    using OuterClass = BitArray;

    // iterator traits
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = typename OuterClass::difference_type;
    using value_type        = typename OuterClass::value_type;
    using pointer           = typename OuterClass::pointer;
    using reference         = typename OuterClass::reference;

    BitArrayIterator() = default;
    inline explicit BitArrayIterator(const OuterClass *instance, uint32_t index = 0)
            : m_instance{instance}, m_index{index} {}


    value_type operator*() const {

        if (!m_instance || m_index >= m_instance->bit_length()) // avoid UB, just die
            throw std::runtime_error{"invalid iterator access"};

        //std::cout << "index: " << m_index << std::endl;
        return m_instance->operator[](m_index);
    }

    BitArrayIterator & operator++() {
        return ++m_index, *this;
    }

    BitArrayIterator operator++(int) {
        auto cpy = *this;
        return ++(*this), cpy;
    }

    BitArrayIterator & operator--() {
        return --m_index, *this;
    }

    BitArrayIterator operator--(int) {
        auto cpy = *this;
        return --(*this), cpy;
    }

    bool operator==(const BitArrayIterator &o) const { return m_index == o.m_index && m_instance == o.m_instance; }
    bool operator!=(const BitArrayIterator &o) const { return !(*this == o); }

    private:
        const OuterClass *m_instance;
        uint64_t m_index;

};

BitArray::const_iterator BitArray::begin() const { return BitArray::const_iterator(this); }
BitArray::const_iterator   BitArray::end() const { return BitArray::const_iterator(this, this->bit_length()); }

BitArray::const_iterator BitArray::cbegin() const { return std::cbegin(*this); }
BitArray::const_iterator   BitArray::cend() const { return std::cend(*this);   }


BitArray & BitArray::operator+=(const BitArray &o) {

    assert(&o != this);

    if (o.bit_length() == 0)
        return *this;

    // we are lucky 'cause we can block-copy
    if (this->bit_length() % 8 == 0) {

        auto sz = effective_byte_size(); // ATTENTION!!! this value can be 0, usually memory size is never 0 i dont wanna change the entire logic
        auto o_sz = o.effective_byte_size();

        m_vct.resize( sz + o_sz ); // 5+1 = 6
        assert(&(o.m_vct) != &(this->m_vct));  // non dovrebbe mai succedere, eventualmente se &o == this usare memmove()

        void *dst = m_vct.data() + sz; // skip N bytes -> buf+5
        memcpy(dst, o.m_vct.data(), o_sz); // cpy(&buf[5], src, 1) -> this copy 1 byte starting from address 5 which is writeable because the size is 6

        this->m_bit_capacity = 8 * m_vct.size();
        this->m_bit_idx += o.m_bit_idx;
        return *this;
    }

    //for (bool bit : o) this->push_back(bit);
    for (uint64_t i = 0, len = o.bit_length(); i < len; ++i)
        this->push_back(o[i]);

    return *this;
}


static_assert(std::bidirectional_iterator<BitArray::BitArrayIterator>);