#pragma once

#include <bit>
#include <fstream>
#include <vector>
#include <string>
#include <limits.h>

class Reader {
    const static size_t DEFAULT_BUFFER_SIZE = 32768;

public:
    class FileReadError : std::exception {};

    explicit Reader(const std::string &file_name, size_t buffer_byte_size = (DEFAULT_BUFFER_SIZE / CHAR_BIT));

    bool ReadBit();

    template <typename T>
    T ReadBits(size_t number_bits);

    void Reload();

    bool IsEof() const;

    std::string GetFileName() const;

private:
    std::string file_name_;
    std::ifstream stream_;
    std::vector<bool> data_;
    size_t next_position_;
    const size_t buffer_size_ = DEFAULT_BUFFER_SIZE;

    bool UpdateBuffer();
};

template <typename T>
T Reader::ReadBits(size_t number_bits) {
    T result = 0;
    for (size_t i = 0; i < number_bits; ++i) {
        result <<= 1;
        result |= static_cast<T>(ReadBit());
    }
    return result;
}