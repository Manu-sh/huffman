#pragma once
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <ios>
#include <sstream>
#include <stdexcept>
#include <string>

// i could use template specialization and sfinae to force inlining
// for some of these global function, but it's unnecessarily
// complicated and less flexible

#ifdef FORCED
    #error "FORCED() macro already defined"
#endif

#define FORCED(_UNUSED_) inline __attribute__((always_inline))


static std::ifstream ifstream_open(const char *filename, std::ios_base::iostate flags = std::ios_base::badbit) {
    using std::literals::string_literals::operator""s, std::ios_base;
    std::ifstream file;
    file.exceptions(flags);
    file.open(filename, ios_base::in|ios_base::binary);
    if (!file) throw std::invalid_argument{"Unable to open file: "s + filename};
    return file;
}

static std::ofstream ofstream_open(const char *filename, std::ios_base::iostate flags = std::ios_base::badbit) {
    using std::literals::string_literals::operator""s, std::ios_base;
    std::ofstream file;
    file.exceptions(flags);
    file.open(filename, ios_base::in|ios_base::binary|ios_base::trunc);
    if (!file) throw std::invalid_argument{"Unable to open file: "s + filename};
    return file;
}

static std::string file_content(std::string_view fname) {

    std::ifstream is = ifstream_open(fname.data());
    const std::size_t size = std::filesystem::file_size(fname);

    std::string buffer(size, '\0');
    is.read(&buffer[0], size);
    return buffer;
}

static std::string stream_content(std::istream &is) {
    std::ostringstream oss;
    oss << is.rdbuf();
    return oss.str();
}