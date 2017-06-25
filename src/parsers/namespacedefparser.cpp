#include "namespacedefparser.hh"
#include "tools.hh"

using namespace nolang;

NamespaceDefParser::NamespaceDefParser(mpc_ast_t *t) :
    tree(t)
{
}

void NamespaceDefParser::reset()
{
    cast = false;
    definitions.clear();
    namespacedef = new NamespaceDef();
}

void NamespaceDefParser::parseIdentifier()
{
    if (cast) namespacedef->setCast(item->contents);
    else definitions.push_back(item->contents);
}

void NamespaceDefParser::parseCast()
{
    cast = true;
}

bool NamespaceDefParser::isIdentifier() const
{
    return expect(item, "identifier");
}

bool NamespaceDefParser::isCast() const
{
    return expect(item, "string", "::");
}

bool NamespaceDefParser::isDot() const
{
    return expect(item, "char", ".");
}

void NamespaceDefParser::parseItem()
{
    if (isIdentifier()) parseIdentifier();
    else if (isCast()) parseCast();
    else if (isDot()); // TODO
    else printError("Unknown node in namespace defination", item);
}

void NamespaceDefParser::destroyResult()
{
    delete namespacedef;
    namespacedef = nullptr;
}

void NamespaceDefParser::setValues()
{
    if (!definitions.empty()) namespacedef->setValues(definitions);
    else destroyResult();
}

NamespaceDef *NamespaceDefParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    setValues();
    return namespacedef;
}
