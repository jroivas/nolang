#include "parser.hh"
#include "test.hh"
#include <string>

TEST_MAIN(
    TEST_SUITE(basic parser,
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

        TEST_CASE(invalid_top_level,
            parser.parse("test.nolang", "+");
            TEST_ASSERT_FALSE(parser.success())
        )

        TEST_CASE(empty_main_fail,
            parser.parse("test.nolang", "main =>");
            TEST_ASSERT_FALSE(parser.success())
        )

        TEST_CASE(empty_main_ok,
            parser.parse("test.nolang", "main =>\n");
            TEST_ASSERT_TRUE(parser.success())
        )

        TEST_CASE(empty_main_with_import,
            parser.parse("test.nolang", "import sys\nmain =>\n");
            TEST_ASSERT_TRUE(parser.success())
        )
    )
)
