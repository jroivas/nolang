#include "numberparser.hh"
#include "tools.hh"

using namespace nolang;

NumberParser::NumberParser(mpc_ast_t *t) :
    tree(t)
{
}

void NumberParser::reset()
{
    negate = false;
    res = nullptr;
    item = tree;
}

bool NumberParser::isNegate() const
{
    return expect(item, "char", "-");
}

bool NumberParser::isNumberValue() const
{
    return res == nullptr && expect(item, "regex");
}

bool NumberParser::isSimpleNumber() const
{
    return expect(item, "number") && !std::string(item->contents).empty();
}

void NumberParser::createNumber()
{
    res = new NumberValue((negate ? "-" : "") + std::string(item->contents));
}

void NumberParser::createSimpleNumber()
{
    res = new NumberValue(item->contents);
}

void NumberParser::parseItem()
{
    if (isNegate()) negate ^= true;
    else if (isNumberValue()) createNumber();
    else printError("Unknown node in number", item);
}

NumberValue *NumberParser::parse()
{
    reset();
    if (isSimpleNumber()) createSimpleNumber();
    else iterate(item, tree, parseItem);
    return res;
}
