#include <catch.hpp>

#include "../src/lib/writer.h"
#include "../src/lib/reader.h"

TEST_CASE("CharsWriteRead") {
    {
        Writer writer("___tmp");
        writer.WriteBits('a', 8);
        writer.WriteBits('b', 8);
        writer.WriteBits('c', 8);
        writer.WriteBits('d', 8);
    }
    {
        Reader reader("___tmp");
        REQUIRE(reader.ReadBits<char>(8) == 'a');
        REQUIRE(reader.ReadBits<char>(8) == 'b');
        REQUIRE(reader.ReadBits<char>(8) == 'c');
        REQUIRE(reader.ReadBits<char>(8) == 'd');
    }
    std::remove("__tmp");
}

TEST_CASE("IntsWriteRead") {
    {
        Writer writer("___tmp");
        writer.WriteBits(1, 15);
        writer.WriteBits(2, 15);
        writer.WriteBits(3, 15);
        writer.WriteBits(4, 15);
    }
    {
        Reader reader("___tmp");
        REQUIRE(reader.ReadBits<int>(15) == 1);
        REQUIRE(reader.ReadBits<int>(15) == 2);
        REQUIRE(reader.ReadBits<int>(15) == 3);
        REQUIRE(reader.ReadBits<int>(15) == 4);
    }
    std::remove("__tmp");
}