#include "parser.hh"
#include "test.hh"
#include <string>

TEST_SUITE(
    nolang::Parser parser;

    TEST_CASE(empty_case,
        parser.parse("test.nolang", "");
        TEST_ASSERT_TRUE(parser.success())
    )

    TEST_CASE(basic_parser,
        std::string data = "";

        data += "main =>\n";
        data += "    40 + 2\n";

        parser.parse("test.nolang", data);

        TEST_ASSERT_TRUE(parser.success())
    )

    TEST_CASE(fail_case,
        parser.parse("test.nolang", "+");
        TEST_ASSERT_FALSE(parser.success())
    )
)
