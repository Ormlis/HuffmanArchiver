#include <catch.hpp>

#include "../src/lib/queue_increasing.h"

TEST_CASE("OnlyIncrease") {
    QueueTwoIncreasing<int> queue;

    queue.Push(0);
    queue.Push(0);
    queue.Push(1);
    queue.Push(1);
    queue.Push(2);
    queue.Push(2);

    REQUIRE(queue.Size() == 6);
    REQUIRE(queue.Top() == 0);
    REQUIRE(queue.ExtractMin() == 0);

    REQUIRE(queue.Size() == 5);
    REQUIRE(queue.Top() == 0);
    REQUIRE(queue.ExtractMin() == 0);

    REQUIRE(queue.Size() == 4);
    REQUIRE(queue.Top() == 1);
    REQUIRE(queue.ExtractMin() == 1);

    REQUIRE(queue.Size() == 3);
    REQUIRE(queue.Top() == 1);
    queue.Pop();

    REQUIRE(queue.Size() == 2);
    REQUIRE(queue.Top() == 2);
    queue.Pop();

    REQUIRE(queue.Size() == 1);
    REQUIRE(queue.Top() == 2);
    queue.Pop();

    REQUIRE(queue.Size() == 0);
}

TEST_CASE("TwoIncrease") {
    QueueTwoIncreasing<int> queue;

    queue.Push(0);
    queue.Push(1);
    queue.Push(2);

    REQUIRE(queue.Size() == 3);
    REQUIRE(queue.Top() == 0);
    REQUIRE(queue.ExtractMin() == 0);

    queue.Push(0);
    queue.Push(1);

    REQUIRE(queue.Size() == 4);
    REQUIRE(queue.Top() == 0);
    REQUIRE(queue.ExtractMin() == 0);

    REQUIRE(queue.Size() == 3);
    REQUIRE(queue.Top() == 1);
    REQUIRE(queue.ExtractMin() == 1);

    REQUIRE(queue.Size() == 2);
    REQUIRE(queue.Top() == 1);
    queue.Pop();

    REQUIRE(queue.Size() == 1);
    REQUIRE(queue.Top() == 2);

    queue.Push(1);

    REQUIRE(queue.Size() == 2);
    REQUIRE(queue.Top() == 1);
    REQUIRE(queue.ExtractMin() == 1);

    REQUIRE(queue.Size() == 1);
    REQUIRE(queue.Top() == 2);
    REQUIRE(queue.ExtractMin() == 2);

    REQUIRE(queue.Size() == 0);
}

TEST_CASE("StringQueue") {
    QueueTwoIncreasing<std::string> queue;
    queue.Push("0");
    queue.Push("0");
    queue.Push("1");
    queue.Push("1");
    queue.Push("2");
    queue.Push("2");

    REQUIRE(queue.Size() == 6);
    REQUIRE(queue.Top() == "0");
    REQUIRE(queue.ExtractMin() == "0");

    REQUIRE(queue.Size() == 5);
    REQUIRE(queue.Top() == "0");
    REQUIRE(queue.ExtractMin() == "0");

    REQUIRE(queue.Size() == 4);
    REQUIRE(queue.Top() == "1");
    REQUIRE(queue.ExtractMin() == "1");

    REQUIRE(queue.Size() == 3);
    REQUIRE(queue.Top() == "1");
    queue.Pop();

    REQUIRE(queue.Size() == 2);
    REQUIRE(queue.Top() == "2");
    queue.Pop();

    REQUIRE(queue.Size() == 1);
    REQUIRE(queue.Top() == "2");
    queue.Pop();

    REQUIRE(queue.Size() == 0);
}