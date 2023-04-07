#include <catch.hpp>

#include "../src/lib/trie.h"

#include <vector>
#include <string>

TEST_CASE("FindString") {
    Trie<size_t, 256> trie;
    const std::vector<std::string> paths = {"abcd", "bcde", "af", "ae", "bee"};
    for (size_t i = 0; i < paths.size(); ++i) {
        trie.Insert(paths[i], i + 1);
    }

    std::string path = "aeabcdbee";
    size_t current_index = 0;
    auto get_next = [&]() { return path[current_index++]; };
    REQUIRE(*trie.TraverseOnce(get_next) == 4);
    REQUIRE(*trie.TraverseOnce(get_next) == 1);
    REQUIRE(*trie.TraverseOnce(get_next) == 5);

    REQUIRE(trie.Traverse(std::begin(path), std::end(path)) == std::vector<size_t>{4, 1, 5});
}

TEST_CASE("NotFound") {
    Trie<size_t, 256> trie;
    const std::vector<std::string> paths = {"abcd", "bcde", "af", "ae", "bee"};
    for (size_t i = 0; i < paths.size(); ++i) {
        trie.Insert(paths[i], i + 1);
    }

    std::string path = "afbeebfde";
    size_t current_index = 0;
    auto get_next = [&]() { return path[current_index++]; };
    REQUIRE(*trie.TraverseOnce(get_next) == 3);
    REQUIRE(*trie.TraverseOnce(get_next) == 5);
    REQUIRE(trie.TraverseOnce(get_next) == nullptr);

    REQUIRE(trie.Traverse(std::begin(path), std::end(path)) == std::vector<size_t>{3, 5});
}