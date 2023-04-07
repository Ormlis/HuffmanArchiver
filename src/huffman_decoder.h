#pragma once

#include "huffman_constants.h"

#include "lib/trie.h"
#include "lib/reader.h"
#include "lib/writer.h"
#include "decode_trie.h"

#include <unordered_map>

template <typename T = huffman::DEFAULT_CHAR_TYPE, size_t IN_CHAR_SIZE = huffman::DEFAULT_IN_CHAR_SIZE,
          size_t OUT_CHAR_SIZE = huffman::DEFAULT_OUT_CHAR_SIZE>
class HuffmanDecoder {
    using CharTrie = Trie<T, 2>;

public:
    class FailedDecodeException : public std::exception {};

    bool Decode(Reader &reader) {
        try {
            while (DecodeFile(reader)) {
            }
        } catch (const FailedDecodeException &e) {
            return false;
        }
        return true;
    }

private:
    CharTrie ReadHuffmanData(Reader &reader) {
        const size_t symbols_count = reader.ReadBits<T>(OUT_CHAR_SIZE);

        std::vector<std::pair<size_t, T>> canonical_order(symbols_count);
        for (auto &[code_length, character] : canonical_order) {
            character = reader.ReadBits<T>(OUT_CHAR_SIZE);
        }

        size_t current_char_index = 0;
        size_t code_size = 1;
        while (current_char_index < symbols_count) {
            size_t number_symbol_with_code_size = reader.ReadBits<T>(OUT_CHAR_SIZE);
            for (size_t j = 0; j < number_symbol_with_code_size; ++j) {
                canonical_order[current_char_index].first = code_size;
                ++current_char_index;
            }
            ++code_size;
        }
        return DecodeTrie(canonical_order);
    }

    std::string DecodeFileName(Reader &reader, CharTrie &trie) {
        std::string file_name;
        while (true) {
            auto current_char_ptr = trie.TraverseOnce([&reader]() { return reader.ReadBit(); });
            if (current_char_ptr == nullptr) {
                throw FailedDecodeException();
            }
            if (*current_char_ptr == huffman::FILENAME_END) {
                break;
            }
            file_name += *current_char_ptr;
        }
        return file_name;
    }

    bool DecodeFile(Reader &reader) {
        CharTrie trie = ReadHuffmanData(reader);

        std::string file_name = DecodeFileName(reader, trie);
        Writer writer(file_name);

        while (true) {
            auto current_char_ptr = trie.TraverseOnce([&reader]() { return reader.ReadBit(); });
            if (current_char_ptr == nullptr) {
                writer.Clear();
                throw FailedDecodeException();
            }
            if (*current_char_ptr == huffman::ARCHIVE_END || *current_char_ptr == huffman::ONE_MORE_FILE) {
                return *current_char_ptr == huffman::ONE_MORE_FILE;
            }
            writer.WriteBits(*current_char_ptr, IN_CHAR_SIZE);
        }
        return false;
    }
};