#pragma once

#include <3pp/mpc/mpc.h>
#include "puremethod.hh"
#include "compiler.hh"

namespace nolang {

class MethodParser
{
public:
    MethodParser(mpc_ast_t *);
    PureMethod *parse();

private:
    void reset();

    bool isPure() const;
    bool isIdentifier() const;

    void parseIdentifier();
    void parseCast();
    void parseItem();

    mpc_ast_t *tree;
    mpc_ast_t *item;

    bool waitName;
    bool waitBody;

    PureMethod *method;
};

}


