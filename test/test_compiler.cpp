#include "compiler.hh"
#include "parser.hh"
#include "test.hh"
#include <string>

TEST_MAIN(
    TEST_SUITE(basic compile,
        nolang::Parser parser;
        nolang::Compiler compiler;

        TEST_CASE(basic_compile,
            std::string data = "";
            data += "main =>\n";
            data += "    40 + 2\n";

            auto res = parser.parse("test.nolang", data);
            TEST_ASSERT_TRUE(parser.success())

            auto statements = compiler.codegen(static_cast<mpc_ast_t*>(res->output));
        )

        TEST_CASE(basic_compile_imports, TEST_ASSERT_EQUALS(compiler.imports().size(), 0))
        TEST_CASE(basic_compile_methods, TEST_ASSERT_EQUALS(compiler.methods().size(), 1))

        nolang::PureMethod *main = compiler.methods()["main"];

        TEST_CASE(basic_compile_main_method, TEST_ASSERT_TRUE(main != nullptr))
        TEST_CASE(basic_compile_main_name, TEST_ASSERT_EQUALS(main->name(),  "main"))
        TEST_CASE(basic_compile_main_return_type, TEST_ASSERT_EQUALS(main->returnType()->code(), "void"))
        TEST_CASE(basic_compile_main_variables_size, TEST_ASSERT_EQUALS(main->variables().size(), 0))
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

    TEST_SUITE(println compile,
        nolang::Parser parser;
        nolang::Compiler compiler;

        mpc_result_t *res = nullptr;
        TEST_CASE(parse_println,
            std::string data = "";
            data += "import IO\n";
            data += "main =>\n";
            data += "    life : int32 = 43 - 1\n";
            data += "    IO.println(life)\n";

            res = parser.parse("test.nolang", data);
            TEST_ASSERT_TRUE(parser.success())
        )

        TEST_CASE(compile_println,
            compiler = nolang::Compiler();

            auto statements = compiler.codegen(static_cast<mpc_ast_t*>(res->output));
        )
        nolang::PureMethod *main = compiler.methods()["main"];

        TEST_CASE(basic_compile_imports, TEST_ASSERT_EQUALS(compiler.imports().size(), 1))
        TEST_CASE(basic_compile_import1_name, TEST_ASSERT_EQUALS(compiler.imports()[0], "IO"))
        TEST_CASE(basic_compile_methods, TEST_ASSERT_EQUALS(compiler.methods().size(), 1))
        TEST_CASE(basic_compile_main_name, TEST_ASSERT_EQUALS(main->name(),  "main"))
        TEST_CASE(basic_compile_main_return_type, TEST_ASSERT_EQUALS(main->returnType()->code(), "void"))
        TEST_CASE(basic_compile_main_variables_size, TEST_ASSERT_EQUALS(main->variables().size(), 1))
        TEST_CASE(basic_compile_main_variables_size, TEST_ASSERT_EQUALS(main->variables()[0]->code(), "life"))
        TEST_CASE(basic_compile_main_blocks_size, TEST_ASSERT_EQUALS(main->blocks().size(), 2))
        TEST_CASE(basic_compile_main_block1_size, TEST_ASSERT_EQUALS(main->blocks()[0].size(), 1))
        TEST_CASE(basic_compile_main_block1_items_size, TEST_ASSERT_EQUALS(main->blocks()[0][0].size(), 2))
        TEST_CASE(basic_compile_main_block1_item1_type, TEST_ASSERT_EQUALS(main->blocks()[0][0][0]->type(), "Assignment"))

        nolang::Assignment *assignment = static_cast<nolang::Assignment*>(main->blocks()[0][0][0]);

        TEST_CASE(assignment statments size, TEST_ASSERT_EQUALS(assignment->statements().size(), 3))
        TEST_CASE(assignment statments 1 type, TEST_ASSERT_EQUALS(assignment->statements()[0]->type(), "Number"))
        TEST_CASE(assignment statments 1 value, TEST_ASSERT_EQUALS(assignment->statements()[0]->code(), "43"))
        TEST_CASE(assignment statments 2 type, TEST_ASSERT_EQUALS(assignment->statements()[1]->type(), "Op"))
        TEST_CASE(assignment statments 2 value, TEST_ASSERT_EQUALS(assignment->statements()[1]->code(), "-"))
        TEST_CASE(assignment statments 3 type, TEST_ASSERT_EQUALS(assignment->statements()[2]->type(), "Number"))
        TEST_CASE(assignment statments 3 value, TEST_ASSERT_EQUALS(assignment->statements()[2]->code(), "1"))

        TEST_CASE(basic_compile_main_block1_item1_value, TEST_ASSERT_EQUALS(main->blocks()[0][0][0]->code(), "life"))
        TEST_CASE(basic_compile_main_block1_item2_type, TEST_ASSERT_EQUALS(main->blocks()[0][0][1]->type(), "EOS"))

        TEST_CASE(basic_compile_main_block1_items_size, TEST_ASSERT_EQUALS(main->blocks()[1][0].size(), 2))
        TEST_CASE(basic_compile_main_block1_item1_type, TEST_ASSERT_EQUALS(main->blocks()[1][0][0]->type(), "MethodCall"))
        nolang::MethodCall *mcall = static_cast<nolang::MethodCall*>(main->blocks()[1][0][0]);

        TEST_CASE(method call namespaces size, TEST_ASSERT_EQUALS(mcall->namespaces().size(), 2))
        TEST_CASE(method call namespaces 1, TEST_ASSERT_EQUALS(mcall->namespaces()[0], "IO"))
        TEST_CASE(method call namespaces 2, TEST_ASSERT_EQUALS(mcall->namespaces()[1], "println"))


        TEST_CASE(method call params size, TEST_ASSERT_EQUALS(mcall->params().size(), 1))
        TEST_CASE(method call params 1 size, TEST_ASSERT_EQUALS(mcall->params()[0].size(), 1))
        TEST_CASE(method call params 1 type, TEST_ASSERT_EQUALS(mcall->params()[0][0]->type(), "Identifier"))
        TEST_CASE(method call params 1 code, TEST_ASSERT_EQUALS(mcall->params()[0][0]->code(), "life"))

        TEST_CASE(basic_compile_main_block1_item1_type, TEST_ASSERT_EQUALS(main->blocks()[1][0][1]->type(), "EOS"))
    )

    TEST_SUITE(method with params,
        nolang::Parser parser;
        nolang::Compiler compiler;

        mpc_result_t *res = nullptr;
        TEST_CASE(parse,
            std::string data = "";
            data += "test(x : int32, y : int32) : int32 =>\n";
            data += "    x * y\n";
            data += "main =>\n";
            data += "    test(1, 2)\n";

            res = parser.parse("test.nolang", data);
            TEST_ASSERT_TRUE(parser.success())
        )

        TEST_CASE(compile,
            compiler = nolang::Compiler();

            mpc_ast_print(static_cast<mpc_ast_t*>(res->output));
            auto statements = compiler.codegen(static_cast<mpc_ast_t*>(res->output));
        )

        TEST_CASE(basic_compile_imports, TEST_ASSERT_EQUALS(compiler.imports().size(), 0))
        TEST_CASE(basic_compile_methods, TEST_ASSERT_EQUALS(compiler.methods().size(), 2))

        nolang::PureMethod *main = compiler.methods()["main"];
        nolang::PureMethod *test = compiler.methods()["test"];

        TEST_CASE(main method valid, TEST_ASSERT_TRUE(main != nullptr))
        TEST_CASE(main method name, TEST_ASSERT_EQUALS(main->name(),  "main"))

        TEST_CASE(main method return type void, TEST_ASSERT_EQUALS(main->returnType()->code(), "void"))
        TEST_CASE(main method variables size, TEST_ASSERT_EQUALS(main->variables().size(), 0))

        TEST_CASE(test method valid, TEST_ASSERT_TRUE(test != nullptr))
        TEST_CASE(test method name, TEST_ASSERT_EQUALS(test->name(),  "test"))
        TEST_CASE(test method return type, TEST_ASSERT_EQUALS(test->returnType()->code(), "int32"))
        TEST_CASE(test method variables size, TEST_ASSERT_EQUALS(test->variables().size(), 0))
    )
)
