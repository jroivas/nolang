#pragma once

#include <3pp/mpc/mpc.h>
#include "baseparser.hh"
#include "statement.hh"

namespace nolang {

class NumberParser : public BaseParser
{
public:
    NumberParser(mpc_ast_t *);
    NumberValue *parse();

private:
    void reset();

    bool isNegate() const;
    bool isNumberValue() const;
    bool isSimpleNumber() const;

    void createNumber();
    void createSimpleNumber();
    void parseItem();

    mpc_ast_t *tree;
    bool negate;

    NumberValue *res;
};

}

