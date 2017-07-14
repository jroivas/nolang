#include "typeidentparser.hh"
#include "tools.hh"

using namespace nolang;

TypeIdentParser::TypeIdentParser(mpc_ast_t *t) :
    BaseParser(t)
{
}

void TypeIdentParser::reset()
{
    wait_colon = true;
    name = "";
    type = "";
}

void TypeIdentParser::foundColon()
{
    if (!wait_colon) throw std::string("Two colons in type identifier!");
    wait_colon = false;
}

void TypeIdentParser::parseIdentifier()
{
    if (isIdentifierName()) name += item->contents;
    else type += item->contents;
}

bool TypeIdentParser::isIdentifierName() const
{
    return wait_colon;
}

bool TypeIdentParser::isColon() const
{
    return wait_colon && expect(item, "char", ":");
}

void TypeIdentParser::parseItem()
{
    if (isIdentifier()) parseIdentifier();
    else if (isColon()) foundColon();
}

TypeIdent *TypeIdentParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return new TypeIdent(name, type);
}
