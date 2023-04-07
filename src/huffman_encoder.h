#pragma once

#include "huffman_constants.h"

#include "lib/trie.h"
#include "lib/reader.h"
#include "lib/writer.h"
#include "lib/queue_increasing.h"
#include "decode_trie.h"

#include <unordered_map>

template <typename T = huffman::DEFAULT_CHAR_TYPE, size_t IN_CHAR_SIZE = huffman::DEFAULT_IN_CHAR_SIZE,
          size_t OUT_CHAR_SIZE = huffman::DEFAULT_OUT_CHAR_SIZE>
class HuffmanEncoder {
    using CharTrie = Trie<T, 2>;

public:
    void EncodeFile(Reader &reader, Writer &writer, bool is_last = true) {
        CharTrie trie = BuildTrie(reader);
        trie = MakeCanonical(trie);
        WriteEncoded(trie, reader, writer, is_last);
    }

    void EncodeFiles(const std::vector<std::string> &file_names, Writer &writer) {
        for (size_t i = 0; i < file_names.size(); ++i) {
            Reader reader(file_names[i]);
            EncodeFile(reader, writer, (i + 1 == file_names.size()));
        }
    }

private:
    struct PriorityElement {
        CharTrie trie;
        size_t number_occurrences;
        T character;
    };

    struct PriorityElementComparator {
        bool operator()(const PriorityElement &left, const PriorityElement &right) const {
            return std::tie(left.number_occurrences, left.character) <
                   std::tie(right.number_occurrences, right.character);
        }
    } comparator_;

    std::unordered_map<T, size_t> CountOccurrences(Reader &reader) {
        std::unordered_map<T, size_t> character_occurrences;
        for (const T &ch : reader.GetFileName()) {
            ++character_occurrences[ch];
        }

        while (!reader.IsEof()) {
            T value = reader.ReadBits<T>(IN_CHAR_SIZE);
            ++character_occurrences[value];
        }
        character_occurrences[huffman::FILENAME_END] = 1;
        character_occurrences[huffman::ONE_MORE_FILE] = 1;
        character_occurrences[huffman::ARCHIVE_END] = 1;
        reader.Reload();
        return character_occurrences;
    }

    CharTrie BuildTrie(Reader &reader) {
        QueueTwoIncreasing<PriorityElement, PriorityElementComparator> queue;

        std::vector<PriorityElement> init_elements;
        for (auto [character, occurrences] : CountOccurrences(reader)) {
            CharTrie trie;
            trie.Insert(std::vector<size_t>{}, character);  // Make only root terminal (path length = 0)
            init_elements.push_back(
                {.trie = std::move(trie), .number_occurrences = occurrences, .character = character});
        }
        std::sort(init_elements.begin(), init_elements.end(), comparator_);

        for (auto &element : init_elements) {
            queue.Push(std::move(element));
        }

        while (queue.Size() > 1) {
            PriorityElement element1(queue.ExtractMin());
            PriorityElement element2(queue.ExtractMin());
            queue.Push({.trie = CharTrie::Merge(element1.trie, element2.trie),
                        .number_occurrences = element1.number_occurrences + element2.number_occurrences,
                        .character = std::min(element1.character, element2.character)});
        }

        CharTrie trie(queue.ExtractMin().trie);
        return trie;
    }

    CharTrie MakeCanonical(CharTrie &trie) {
        std::vector<std::pair<size_t, T>> canonical_order;
        for (const auto &[code, character] : trie.GetTerminals()) {
            canonical_order.emplace_back(code.size(), character);
        }
        std::sort(canonical_order.begin(), canonical_order.end());

        return DecodeTrie(canonical_order);
    }

    void WriteHuffmanData(CharTrie &trie, Writer &writer) {
        const size_t symbols_count = trie.GetNumberOfTerminals();
        writer.WriteBits(symbols_count, OUT_CHAR_SIZE);

        std::vector<size_t> number_symbol_with_code_size(1);

        for (const auto &[code, character] : trie.GetTerminals()) {
            writer.WriteBits(character, OUT_CHAR_SIZE);

            if (number_symbol_with_code_size.size() <= code.size()) {
                number_symbol_with_code_size.resize(code.size() + 1);
            }
            ++number_symbol_with_code_size[code.size()];
        }

        for (size_t i = 1; i < number_symbol_with_code_size.size(); ++i) {
            writer.WriteBits(number_symbol_with_code_size[i], OUT_CHAR_SIZE);
        }
    }

    void WriteEncoded(CharTrie &trie, Reader &reader, Writer &writer, bool is_last = true) {
        WriteHuffmanData(trie, writer);

        std::unordered_map<T, std::vector<bool>> char_code;
        for (const auto &[code, character] : trie.GetTerminals()) {
            std::vector<bool> bool_code(code.begin(), code.end());
            char_code[character] = bool_code;
        }

        for (const T &ch : reader.GetFileName()) {
            writer.WriteBits(char_code[ch]);
        }
        writer.WriteBits(char_code[huffman::FILENAME_END]);

        while (!reader.IsEof()) {
            T value = reader.ReadBits<T>(IN_CHAR_SIZE);
            writer.WriteBits(char_code[value]);
        }
        if (is_last) {
            writer.WriteBits(char_code[huffman::ARCHIVE_END]);
        } else {
            writer.WriteBits(char_code[huffman::ONE_MORE_FILE]);
        }
        reader.Reload();
    }
};