#pragma once

#include <array>
#include <memory>
#include <vector>

template <typename T, size_t ALPHABET_SIZE>
class Trie {
    using VertexPath = std::pair<std::vector<size_t>, T>;

    class TrieNode {
    public:
        using PtrArray = std::array<std::unique_ptr<TrieNode>, ALPHABET_SIZE>;

        explicit TrieNode() : value_(), is_terminal_(false), children_() {
        }

        void SetValue(T value) {
            value_ = value;
        }

        void SetState(bool is_terminal) {
            is_terminal_ = is_terminal;
        }

        void SetChildren(size_t index, std::unique_ptr<TrieNode> &child) {
            std::swap(children_[index], child);
        }

        template <typename Iterator>
        std::pair<const T *, Iterator> Traverse(Iterator it, Iterator end) const {
            if (is_terminal_) {
                return {&value_, it};
            }
            if (it == end || !children_[*it]) {
                return {nullptr, it};
            }
            return children_[*it]->Traverse(next(it), end);
        }

        const T *Traverse(const auto &get_next) const {
            if (is_terminal_) {
                return &value_;
            }
            size_t edge = get_next();
            if (!children_[edge]) {
                return nullptr;
            }
            return children_[edge]->Traverse(get_next);
        }

        template <typename Iterator>
        bool InsertPath(Iterator begin, Iterator end, T &value) {
            if (begin == end) {
                SetValue(value);
                if (is_terminal_) {
                    return false;
                }
                SetState(true);
                return true;
            }
            if (!children_[*begin]) {
                children_[*begin] = std::make_unique<TrieNode>();
            }
            return children_[*begin]->InsertPath(std::next(begin), end, value);
        }

        std::vector<VertexPath> GetTerminals() const {
            std::vector<VertexPath> result;
            std::vector<size_t> path;
            SearchTerminals(path, result);
            return result;
        }

    private:
        T value_;
        bool is_terminal_;

        PtrArray children_;

        void SearchTerminals(std::vector<size_t> &path, std::vector<VertexPath> &terminals) const {
            if (is_terminal_) {
                terminals.emplace_back(path, value_);
            }
            for (size_t edge = 0; edge < ALPHABET_SIZE; ++edge) {
                if (!children_[edge]) {
                    continue;
                }
                path.push_back(edge);
                children_[edge]->SearchTerminals(path, terminals);
                path.pop_back();
            }
        }
    };

public:
    explicit Trie() : root_(std::make_unique<TrieNode>()), number_terminals_(0) {
    }

    template <typename Sequence>
    void Insert(const Sequence &path, T value = {}) {
        if (root_->InsertPath(std::begin(path), std::end(path), value)) {
            ++number_terminals_;
        }
    }

    template <typename... Args>
    static Trie Merge(Args &...args) {
        Trie result;
        result.AddChildren(0, args...);
        return result;
    }

    size_t GetNumberOfTerminals() const {
        return number_terminals_;
    }

    std::vector<VertexPath> GetTerminals() const {
        return root_->GetTerminals();
    }

    template <typename Iterator>
    std::vector<T> Traverse(Iterator begin, Iterator end) const {
        auto it = begin;
        std::vector<T> result;
        while (it != end) {
            auto [value, it2] = TraverseOnce(it, end);
            if (value == nullptr) {
                break;
            }
            result.push_back(*value);
            it = it2;
        }
        return result;
    }

    const T *TraverseOnce(const auto &get_next) const {
        return root_->Traverse(get_next);
    }

    template <typename Iterator>
    std::pair<const T *, Iterator> TraverseOnce(Iterator begin, Iterator end) const {
        return root_->Traverse(begin, end);
    }

private:
    std::unique_ptr<TrieNode> root_;
    size_t number_terminals_;

    void AddChildren(size_t edge) {
        // Recursion Termination
    }

    template <typename... Args>
    void AddChildren(size_t edge, Trie &child, Args &...args) {
        root_->SetChildren(edge, child.root_);
        number_terminals_ += child.number_terminals_;
        AddChildren(edge + 1, args...);
    }
};