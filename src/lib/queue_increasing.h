#pragma once

#include <queue>

template <typename T, typename Compare = std::less<T>>
class QueueTwoIncreasing {
public:
    void Push(T value);

    T& Top();

    void Pop();

    T ExtractMin();

    size_t Size() const;

    bool Empty() const;

private:
    std::queue<T> queue1_;
    std::queue<T> queue2_;
    Compare comparator_;

    T Extract(std::queue<T>& queue);

    enum class Location { NoWhere, Queue1, Queue2 };

    Location GetTopLocation() const {
        if (queue1_.empty() && queue2_.empty()) {
            return Location::NoWhere;
        }
        if (queue1_.empty()) {
            return Location::Queue2;
        } else if (queue2_.empty()) {
            return Location::Queue1;
        } else if (comparator_(queue1_.front(), queue2_.front())) {
            return Location::Queue1;
        } else {
            return Location::Queue2;
        }
    }
};

template <typename T, typename Compare>
void QueueTwoIncreasing<T, Compare>::Push(T value) {
    if (queue1_.empty() || !comparator_(value, queue1_.back())) {
        queue1_.push(std::move(value));
    } else {
        queue2_.push(std::move(value));
    }
}

template <typename T, typename Compare>
size_t QueueTwoIncreasing<T, Compare>::Size() const {
    return queue1_.size() + queue2_.size();
}

template <typename T, typename Compare>
bool QueueTwoIncreasing<T, Compare>::Empty() const {
    return queue1_.empty() && queue2_.empty();
}

template <typename T, typename Compare>
T QueueTwoIncreasing<T, Compare>::Extract(std::queue<T>& queue) {
    auto tmp = std::move(queue.front());
    queue.pop();
    return tmp;
}

template <typename T, typename Compare>
T QueueTwoIncreasing<T, Compare>::ExtractMin() {
    if (GetTopLocation() == Location::Queue2) {
        return Extract(queue2_);
    } else {
        return Extract(queue1_);
    }
}

template <typename T, typename Compare>
T& QueueTwoIncreasing<T, Compare>::Top() {
    if (GetTopLocation() == Location::Queue2) {
        return queue2_.front();
    } else {
        return queue1_.front();
    }
}

template <typename T, typename Compare>
void QueueTwoIncreasing<T, Compare>::Pop() {
    if (GetTopLocation() == Location::Queue2) {
        queue2_.pop();
    } else {
        queue1_.pop();
    }
}