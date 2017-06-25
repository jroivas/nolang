#pragma once

#include <3pp/mpc/mpc.h>
#include "const.hh"
#include "compiler.hh"

namespace nolang {

class ConstParser {
public:
    ConstParser(Compiler *, mpc_ast_t *);
    Const *parse();

private:
    bool isConst() const;
    bool isAssignment() const;
    bool isValidAssignment() const;

    void reset();
    void parseItem();
    void parseAssignment();
    void generateConst();

    Compiler *compiler;
    mpc_ast_t *tree;
    mpc_ast_t *item;
    Const *res;
    PureMethod tmpMethod;
    Assignment *assignment;

    bool wait_const;
};

}

