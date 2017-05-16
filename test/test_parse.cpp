#include "parser.hh"
#include "test.hh"
#include <string>

TEST_SUITE(
    nolang::Parser parser;

    TEST_CASE(basic_parser,
        std::string data = "";

        data += "main =>\n";
        data += "    40 + 2";

        parser.parse("test.nolang", data);

        TEST_ASSERT_TRUE(parser.success())
    )

)
