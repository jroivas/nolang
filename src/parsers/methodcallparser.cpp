#include "methodcallparser.hh"
#include "parsers/namespacedefparser.hh"
#include "tools.hh"

using namespace nolang;

MethodCallParser::MethodCallParser(Compiler *c, mpc_ast_t *t) :
    compiler(c),
    tree(t)
{
}

void MethodCallParser::reset()
{
    wait_ns = true;
    wait_call_end = false;
    mcall = new MethodCall();
}

bool MethodCallParser::isIdentifier() const
{
    return wait_ns && expect(item, "identifier");
}

bool MethodCallParser::isNamespaceDef() const
{
    return wait_ns && expect(item, "namespacedef");
}

bool MethodCallParser::isOpenBrace() const
{
    return !wait_call_end && expect(item, "char", "(");
}

bool MethodCallParser::isCloseBrace() const
{
    return wait_call_end && expect(item, "char", ")");
}

bool MethodCallParser::isParameter() const
{
    return wait_call_end;
}

void MethodCallParser::parseIdentifier()
{
    mcall->setNamespace(new NamespaceDef(item->contents));
    wait_ns = false;
}

void MethodCallParser::parseNamespaceDef()
{
    mcall->setNamespace(NamespaceDefParser(item).parse());
    wait_ns = false;
}

void MethodCallParser::parseParameter()
{
    mcall->addParameter(compiler->codegen(item));
}

void MethodCallParser::parseItem()
{
    if (isIdentifier()) parseIdentifier();
    else if (isNamespaceDef()) parseNamespaceDef();
    else if (isOpenBrace()) wait_call_end = true;
    else if (isCloseBrace()) wait_call_end = false;
    else if (isParameter()) parseParameter();
    else printError("Unknown node in method call", item);
}

MethodCall *MethodCallParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return mcall;
}
