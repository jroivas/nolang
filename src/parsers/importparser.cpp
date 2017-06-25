#include "importparser.hh"
#include "tools.hh"

using namespace nolang;

ImportParser::ImportParser(mpc_ast_t *t) :
    tree(t)
{
}

void ImportParser::reset()
{
    res = nullptr;
}

bool ImportParser::isAs()
{
    return expect(item, "identifier");
}

void ImportParser::createImport()
{
    res = new Import(item->contents);
}

void ImportParser::addAs()
{
    res->addAs(item->contents);
}

void ImportParser::parseAs()
{
    if (isAs()) addAs();
    else createImport();
}

void ImportParser::parseItem()
{
    if (isAs()) parseAs();
    else printError("Unknown node in import", item);
}

Import *ImportParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return res;
}
