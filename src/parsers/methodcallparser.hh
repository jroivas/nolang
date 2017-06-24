#pragma once

#include <3pp/mpc/mpc.h>
#include "methodcall.hh"
#include "compiler.hh"

namespace nolang {

class MethodCallParser {
public:
    MethodCallParser(Compiler *, mpc_ast_t *);
    MethodCall *parse();

private:
    bool isIdentifier() const;
    bool isNamespaceDef() const;
    bool isOpenBrace() const;
    bool isCloseBrace() const;
    bool isParameter() const;

    void reset();
    void parseItem();
    void parseIdentifier();
    void parseNamespaceDef();
    void parseParameter();

    Compiler *compiler;
    mpc_ast_t *tree;
    mpc_ast_t *item;
    MethodCall *mcall;

    bool wait_ns;
    bool wait_call_end;
};

}
