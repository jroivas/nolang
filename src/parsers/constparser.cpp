#include "constparser.hh"
#include "tools.hh"
#include "puremethod.hh"
#include "parsers/assignmentparser.hh"

using namespace nolang;

ConstParser::ConstParser(Compiler *c, mpc_ast_t *t) :
    compiler(c),
    tree(t)
{
}

void ConstParser::reset()
{
    wait_const = true;
    res = nullptr;
}

bool ConstParser::isConst() const
{
    return expect(item, "string", "const");
}

bool ConstParser::isAssignment() const
{
    return !wait_const & expect(item, "assignment");
}

bool ConstParser::isValidAssignment() const
{
    return assignment && tmpMethod.variables().size() == 1;
}

void ConstParser::generateConst()
{
    res = new Const(tmpMethod.variables()[0], assignment);
}

void ConstParser::parseAssignment()
{
    assignment = AssignmentParser(compiler, item, &tmpMethod).parse();
    if (isValidAssignment()) generateConst();
    else printError("Invalid const", item);
}

void ConstParser::parseItem()
{
    if (isConst()) wait_const = false;
    else if (isAssignment()) parseAssignment();
    else printError("Unknown node in const defination", item);
}

Const *ConstParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return res;
}
