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
    gotAs = false;
}

bool ImportParser::isImport() const
{
    return res != nullptr;
}

bool ImportParser::isImportString() const
{
    return expect(item, "string","import");
}

void ImportParser::createImport()
{
    res = new Import(item->contents);
}

void ImportParser::addAs()
{
    if (!gotAs) throw std::string("Invalid import: did not get 'as'.");
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
        else if (isDot()); //FIXME
        else printError("Unknown node in import as", item);
    });
}

void ImportParser::parseItem()
{
    if (isImportString());
    else if (isAs()) gotAs = true;
    else if (isIdentifier()) parseAs();
    else if (isSub()) parseSub();
    else if (isWhitespace() || isNewLine());
    else printError("Unknown node in import", item);
}

Import *ImportParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return res;
}
