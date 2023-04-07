#pragma once

#include "lib/trie.h"
#include "utils.h"

template <typename T>
Trie<T, 2> DecodeTrie(std::vector<std::pair<size_t, T>> canonical_order) {
    Trie<T, 2> canonical_trie;
    std::vector<bool> path;
    for (const auto &[code_length, character] : canonical_order) {
        path.resize(code_length);
        canonical_trie.Insert(path, character);
        IncreasePath(path);
    }
    return canonical_trie;
}