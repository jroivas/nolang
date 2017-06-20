#include "methodparser.hh"
#include "tools.hh"

using namespace nolang;

MethodParser::MethodParser(mpc_ast_t *t) :
    tree(t)
{
}

void MethodParser::reset()
{
    bool waitName = true;
    bool waitBody = false;
    method = new PureMethod();
}

bool MethodParser::isPure() const
{
    return waitName && expect(item, "pure")
}

bool MethodParser::isIdentifier() const
{
    return waitName && expect(item, "identifier")
}

void MethodParser::parseIdentifier()
{
    method->setName(item->contents);
    waitName = false;
}

void MethodParser::parseItem()
{
    if (isPure()) method->setPure();
    if (isIdentifier()) parseIdentifier();
    else printError("Unknown node in method", item);
}

PureMethod *MethodParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return method;
}

