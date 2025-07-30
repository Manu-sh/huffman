#pragma once
#include <cstdint>

// i could use template specialization and sfinae to force inlining
// for some of these global function, but it's unnecessarily
// complicated and less flexible

#ifdef FORCED
    #error "FORCED() macro already defined"
#endif

#define FORCED(_UNUSED_) __attribute__((always_inline))


// ceil_div(x, 8) -> same of (int)ceil(x/8.)
static constexpr FORCED(inline) uint64_t ceil_div(uint64_t num, uint8_t div) noexcept {
    return (num-1) / div + 1;
}
