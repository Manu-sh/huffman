#pragma once
#include <memory>
#include <bitarray/BitArray.hpp>

struct BitStream {

    public:
        BitStream() = default;
        BitStream(std::shared_ptr<BitArray> self): m_self{self} {}

        inline const auto & borrow() const { return *m_self; }
        inline auto share() const { return m_self; }

    protected:
        std::shared_ptr<BitArray> m_self;
};
