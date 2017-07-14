#include "structparser.hh"
#include "parsers/typeidentparser.hh"
#include "tools.hh"

using namespace nolang;

StructParser::StructParser(mpc_ast_t *t) :
    BaseParser(t)
{
}

void StructParser::reset()
{
    res = nullptr;
    got_struct = false;
    begin_struct = false;
    end_struct = false;
}

bool StructParser::isStruct()
{
    return got_struct && !res && expect(item, "identifier");
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
    if (!begin_struct || end_struct) throw std::string("Struct not open!");
    res->addData(TypeIdentParser(item).parse());
}

void StructParser::parseItem()
{
    if (isStructString()) got_struct = true;
    else if (isStruct()) createStruct();
    else if (isBraceOpen()) begin_struct = true;
    else if (isBraceClose()) end_struct = true;
    else if (isData()) parseData();
    else if (isWhitespace() || isNewLine());
    else printError("Unknown node in struct", item);
}
