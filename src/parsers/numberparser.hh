#pragma once

#include <3pp/mpc/mpc.h>
#include "baseparser.hh"
#include "statement.hh"

namespace nolang {

class NumberParser : public BaseParser<NumberValue>
{
public:
    NumberParser(mpc_ast_t *);

private:
    void reset();
    bool isIterable();
    void nonIterableAction();

    bool isNegate() const;
    bool isNumberValue() const;
    bool isSimpleNumber() const;

    void createNumber();
    void createSimpleNumber();
    void parseItem();

    bool negate;
};

}

