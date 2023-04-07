#include "reader.h"

Reader::Reader(const std::string &file_name, size_t buffer_byte_size)
    : file_name_(file_name), stream_(file_name), data_(), next_position_(0), buffer_size_(buffer_byte_size * CHAR_BIT) {
    data_.reserve(buffer_size_);
    UpdateBuffer();
}

bool Reader::ReadBit() {
    if (next_position_ >= data_.size()) {
        if (!UpdateBuffer()) {
            throw FileReadError();
        }
    }
    return data_[next_position_++];
}

void Reader::Reload() {
    stream_.clear();
    stream_.seekg(0);
    data_.clear();
    next_position_ = 0;
    UpdateBuffer();
}

bool Reader::IsEof() const {
    return next_position_ >= data_.size() && stream_.eof();
}

std::string Reader::GetFileName() const {
    return file_name_;
}

bool Reader::UpdateBuffer() {
    std::string char_data(buffer_size_ / CHAR_BIT, 0);
    stream_.read(char_data.data(), static_cast<std::streamsize>(char_data.size()));

    std::streamsize new_size = stream_.gcount();
    data_.resize(new_size * CHAR_BIT);
    next_position_ = 0;
    for (size_t i = 0; i < new_size; ++i) {
        static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);
        if constexpr (std::endian::native == std::endian::big) {
            for (size_t j = 0; j < CHAR_BIT; ++j) {
                data_[i * CHAR_BIT + j] = (char_data[i] >> j) & 1;
            }
        } else {
            for (size_t j = 0; j < CHAR_BIT; ++j) {
                data_[i * CHAR_BIT + j] = (char_data[i] >> (CHAR_BIT - 1 - j)) & 1;
            }
        }
    }
    return !data_.empty();
}
