#include "assignment.hh"
#include "compiler.hh"
#include "parser.hh"
#include "parsers/assignmentparser.hh"
#include "tools.hh"
#include "test.hh"
#include <string>

TEST_MAIN(
    TEST_SUITE(simple assign,
        nolang::Parser parser;
        nolang::Compiler compiler;
        mpc_result_t *res = nullptr;

        TEST_CASE(AssignmentParser: assign parse app,
            std::string data = "";
            data += "main =>\n";
            data += "    v : int32 = 5\n";

            res = parser.parse("test.nolang", data);
            TEST_ASSERT_TRUE(parser.success())
        )

        nolang::Assignment *assignment = nullptr;
        mpc_ast_t *asstree = nullptr;
        TEST_CASE(AssignmentParser: find assignment,
            mpc_ast_t *output = static_cast<mpc_ast_t*>(res->output);
            asstree = nolang::findFirstItemFromTree(output, "assignment");

            TEST_ASSERT_TRUE(asstree != nullptr);
        )

        TEST_CASE(AssignmentParser: assign parse assignment,
            nolang::AssignmentParser aparser(&compiler, asstree, nullptr);
            assignment = aparser.parse();
            TEST_ASSERT_TRUE(assignment != nullptr);
        )

        TEST_CASE(AssignmentParser: assign variable name, TEST_ASSERT_EQUALS(assignment->code(), "v"))
        TEST_CASE(AssignmentParser: assign definition not found, TEST_ASSERT_TRUE(assignment->def() == nullptr))
        TEST_CASE(AssignmentParser: assign statements valid, TEST_ASSERT_TRUE(assignment->statements().size() == 1))
        TEST_CASE(AssignmentParser: assign statements valid, TEST_ASSERT_EQUALS(assignment->statements()[0]->type(), "Number"))
        TEST_CASE(AssignmentParser: assign statements valid, TEST_ASSERT_EQUALS(assignment->statements()[0]->code(), "5"))
    )

    TEST_SUITE(assign namespacedef,
        nolang::Parser parser;
        nolang::Compiler compiler;
        mpc_result_t *res = nullptr;

        TEST_CASE(AssignmentParser: assign parse app,
            std::string data = "";
            data += "main =>\n";
            data += "    tst.num = 42\n";

            res = parser.parse("test.nolang", data);
            TEST_ASSERT_TRUE(parser.success())
        )

        nolang::Assignment *assignment = nullptr;
        mpc_ast_t *asstree = nullptr;
        TEST_CASE(AssignmentParser: find assignment,
            mpc_ast_t *output = static_cast<mpc_ast_t*>(res->output);
            asstree = nolang::findFirstItemFromTree(output, "assignment");

            TEST_ASSERT_TRUE(asstree != nullptr);
        )

        TEST_CASE(AssignmentParser: assign parse assignment,
            nolang::AssignmentParser aparser(&compiler, asstree, nullptr);
            assignment = aparser.parse();
            TEST_ASSERT_TRUE(assignment != nullptr);
        )

        TEST_CASE(AssignmentParser: assign variable name, TEST_ASSERT_EQUALS(assignment->code(), "tst.num"))
        TEST_CASE(AssignmentParser: assign definition found, TEST_ASSERT_TRUE(assignment->def() != nullptr))
        TEST_CASE(AssignmentParser: assign definition name, TEST_ASSERT_EQUALS(assignment->def()->name(), "tst.num"))
        TEST_CASE(AssignmentParser: assign definition name, TEST_ASSERT_EQUALS(assignment->def()->values().size(), 2))
        TEST_CASE(AssignmentParser: assign definition name, TEST_ASSERT_EQUALS(assignment->def()->values()[0], "tst"))
        TEST_CASE(AssignmentParser: assign definition name, TEST_ASSERT_EQUALS(assignment->def()->values()[1], "num"))
        TEST_CASE(AssignmentParser: assign statements valid, TEST_ASSERT_TRUE(assignment->statements().size() == 1))
        TEST_CASE(AssignmentParser: assign statements valid, TEST_ASSERT_EQUALS(assignment->statements()[0]->type(), "Number"))
        TEST_CASE(AssignmentParser: assign statements valid, TEST_ASSERT_EQUALS(assignment->statements()[0]->code(), "42"))
    )
)
