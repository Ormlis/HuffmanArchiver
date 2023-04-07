#include "writer.h"

#include <bit>
#include <fstream>
#include <vector>
#include <limits.h>

Writer::Writer(const std::string& file_name, size_t buffer_byte_size)
    : file_name_(file_name), stream_(file_name), data_(), buffer_size_(buffer_byte_size * CHAR_BIT) {
    data_.reserve(buffer_size_);
}

void Writer::WriteBit(bool value) {
    data_.push_back(value);
    if (data_.size() >= buffer_size_) {
        UpdateBuffer();
    }
}

void Writer::WriteBits(const std::vector<bool>& value) {
    for (auto bit : value) {
        WriteBit(bit);
    }
}

void Writer::Clear() {
    data_.clear();
    stream_.close();
    stream_.open(file_name_);
}

Writer::~Writer() {
    UpdateBuffer();
}

bool Writer::UpdateBuffer() {
    std::string char_data((data_.size() + CHAR_BIT - 1) / CHAR_BIT, 0);
    for (size_t i = 0; i < data_.size(); ++i) {
        static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);
        if constexpr (std::endian::native == std::endian::big) {
            char_data[i / CHAR_BIT] |= static_cast<char>(data_[i]) << (i % CHAR_BIT);
        } else {
            char_data[i / CHAR_BIT] |= static_cast<char>(data_[i]) << (CHAR_BIT - 1 - i % CHAR_BIT);
        }
    }
    stream_.write(char_data.data(), static_cast<std::streamsize>(char_data.size()));
    data_.clear();
    return !char_data.empty();
}
