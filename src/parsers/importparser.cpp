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

bool ImportParser::isIdentifier() const
{
    return expect(item, "identifier");
}

bool ImportParser::isSub() const
{
    return expect(item, "namespacedef");
}

bool ImportParser::isImport() const
{
    return res != nullptr;
}

void ImportParser::createImport()
{
    res = new Import(item->contents);
}

void ImportParser::addAs()
{
    res->addAs(item->contents);
}

void ImportParser::addSub()
{
    res->addSub(item->contents);
}

void ImportParser::parseAs()
{
    if (isImport()) addAs();
    else createImport();
}

void ImportParser::parseSubIdentifier()
{
    if (isImport()) addSub();
    else createImport();
}

void ImportParser::parseSub()
{
    mpc_ast_t *tmp = item;
    iterateTree(tmp, [&] (mpc_ast_t *sub) {
        item = sub;
        if (isIdentifier()) parseSubIdentifier();
        else printError("Unknown node in import as", item);
    });
}

void ImportParser::parseItem()
{
    if (isIdentifier()) parseAs();
    else if (isSub()) parseSub();
    else printError("Unknown node in import", item);
}

Import *ImportParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return res;
}
