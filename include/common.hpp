#pragma once
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <ios>

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


static std::ifstream ifstream_open(const char *filename, std::ios_base::iostate flags = std::ios_base::badbit) {
    using std::literals::string_literals::operator""s, std::ios_base;
    std::ifstream fpnm;
    fpnm.exceptions(flags);
    fpnm.open(filename, ios_base::in|ios_base::binary);
    if (!fpnm) throw std::invalid_argument{"Unable to open file: "s + filename};
    return fpnm;
}

static std::string file_content(std::string_view fname) {

    std::ifstream is = ifstream_open(fname.data());
    const std::size_t size = std::filesystem::file_size(fname);

    std::string buffer(size, '\0');
    is.read(&buffer[0], size);
    return buffer;
}