#include "compiler.hh"
#include "parser.hh"
#include "test.hh"
#include <string>

TEST_SUITE(
    nolang::Parser parser;
    nolang::Compiler compiler;

    TEST_CASE(basic_compile,
        std::string data = "";
        data += "main =>\n";
        data += "    40 + 2\n";

        auto res = parser.parse("test.nolang", data);
        auto statements = compiler.codegen(static_cast<mpc_ast_t*>(res->output));

        TEST_ASSERT_TRUE(parser.success())
    )

    TEST_CASE(basic_compile_imports, TEST_ASSERT_EQUALS(compiler.imports().size(), 0))
    TEST_CASE(basic_compile_methods, TEST_ASSERT_EQUALS(compiler.methods().size(), 1))

    nolang::PureMethod *main = compiler.methods()["main"];

    TEST_CASE(basic_compile_main_method, TEST_ASSERT_TRUE(main != nullptr))
    TEST_CASE(basic_compile_main_name, TEST_ASSERT_EQUALS(main->name(),  "main"))
    TEST_CASE(basic_compile_main_return_type, TEST_ASSERT_EQUALS(main->returnType()->code(), "void"))
    TEST_CASE(basic_compile_main_blocks_size, TEST_ASSERT_EQUALS(main->blocks().size(), 1))
    TEST_CASE(basic_compile_main_block1_size, TEST_ASSERT_EQUALS(main->blocks()[0].size(), 1))
    TEST_CASE(basic_compile_main_block1_items_size, TEST_ASSERT_EQUALS(main->blocks()[0][0].size(), 4))
    TEST_CASE(basic_compile_main_block1_item1_type, TEST_ASSERT_EQUALS(main->blocks()[0][0][0]->type(), "Number"))
    TEST_CASE(basic_compile_main_block1_item1_value, TEST_ASSERT_EQUALS(main->blocks()[0][0][0]->code(), "40"))
    TEST_CASE(basic_compile_main_block1_item1_type, TEST_ASSERT_EQUALS(main->blocks()[0][0][1]->type(), "Op"))
    TEST_CASE(basic_compile_main_block1_item1_value, TEST_ASSERT_EQUALS(main->blocks()[0][0][1]->code(), "+"))
    TEST_CASE(basic_compile_main_block1_item1_type, TEST_ASSERT_EQUALS(main->blocks()[0][0][2]->type(), "Number"))
    TEST_CASE(basic_compile_main_block1_item1_value, TEST_ASSERT_EQUALS(main->blocks()[0][0][2]->code(), "2"))
    TEST_CASE(basic_compile_main_block1_item1_type, TEST_ASSERT_EQUALS(main->blocks()[0][0][3]->type(), "EOS"))
)
