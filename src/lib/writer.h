#pragma once

#include <fstream>
#include <vector>
#include <limits.h>

class Writer {
    const static size_t DEFAULT_BUFFER_SIZE = 32768;

public:
    explicit Writer(const std::string &file_name, size_t buffer_byte_size = (DEFAULT_BUFFER_SIZE / CHAR_BIT));

    void WriteBit(bool value);

    template <typename T>
    void WriteBits(T value, size_t number_bits);

    void WriteBits(const std::vector<bool> &value);

    void Clear();

    ~Writer();

private:
    std::string file_name_;
    std::ofstream stream_;
    std::vector<bool> data_;
    const size_t buffer_size_ = DEFAULT_BUFFER_SIZE;

    bool UpdateBuffer();
};

template <typename T>
void Writer::WriteBits(T value, size_t number_bits) {
    for (size_t i = number_bits; i > 0; --i) {
        WriteBit((value >> (i - 1)) & 1);
    }
}
