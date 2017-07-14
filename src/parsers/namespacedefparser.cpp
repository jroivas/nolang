#include "namespacedefparser.hh"
#include "tools.hh"

using namespace nolang;

NamespaceDefParser::NamespaceDefParser(mpc_ast_t *t) :
    BaseParser(t)
{
}

void NamespaceDefParser::reset()
{
    cast = false;
    definitions.clear();
    res = new NamespaceDef();
}

void NamespaceDefParser::parseIdentifier()
{
    if (cast) res->setCast(item->contents);
    else definitions.push_back(item->contents);
}

void NamespaceDefParser::parseCast()
{
    cast = true;
}

void NamespaceDefParser::parseItem()
{
    if (isIdentifier()) parseIdentifier();
    else if (isCast()) parseCast();
    else if (isDot()); // TODO
    else if (isWhitespace());
    else printError("Unknown node in namespace defination", item);
}

void NamespaceDefParser::destroyResult()
{
    delete res;
    res = nullptr;
}

void NamespaceDefParser::setValues()
{
    if (!definitions.empty()) res->setValues(definitions);
    else destroyResult();
}

void NamespaceDefParser::postProcess()
{
    setValues();
}
