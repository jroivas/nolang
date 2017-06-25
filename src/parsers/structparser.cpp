#include "structparser.hh"
#include "parsers/typeidentparser.hh"
#include "tools.hh"

using namespace nolang;

StructParser::StructParser(mpc_ast_t *t) :
    tree(t)
{
}

void StructParser::reset()
{
    res = nullptr;
}

bool StructParser::isStruct()
{
    return !res && expect(item, "identifier");
}

bool StructParser::isData()
{
    return res && expect(item, "typeident");
}

void StructParser::createStruct()
{
    res = new Struct(item->contents);
}

void StructParser::parseData()
{
    res->addData(TypeIdentParser(item).parse());
}

void StructParser::parseItem()
{
    if (isStruct()) createStruct();
    else if (isData()) parseData();
    else printError("Unknown node in struct", item);
}

Struct *StructParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return res;
}
