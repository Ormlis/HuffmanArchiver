#include <catch.hpp>

#include "../src/lib/cla_parser.h"

TEST_CASE("IntArgument") {
    CLAParser parser;
    parser.AddArgument<int>('i', "int", "integer", "help", false);
}

TEST_CASE("StringArgument") {
    CLAParser parser;
    parser.AddArgument<std::string>('s', "str", "string", "help", false);
}

TEST_CASE("MultiStringArgument") {
    CLAParser parser;
    parser.AddMultipleArguments<std::string>("string", "help", false);
}

TEST_CASE("MultiIntArgument") {
    CLAParser parser;
    parser.AddMultipleArguments<int>("integer", "help", false);
}

TEST_CASE("FlagArgument") {
    CLAParser parser;
    parser.AddFlag('f', "flag", "help");
}