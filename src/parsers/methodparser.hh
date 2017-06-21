#pragma once

#include <3pp/mpc/mpc.h>
#include "puremethod.hh"
#include "compiler.hh"

namespace nolang {

class MethodParser
{
public:
    MethodParser(Compiler *, mpc_ast_t *);
    PureMethod *parse();

private:
    void reset();

    bool isPure() const;
    bool isIdentifier() const;
    bool isArguments() const;
    bool isMethodReturn() const;
    bool isOptionalWhitespace() const;
    bool isBodyStart() const;
    bool isBody() const;
    bool isWhitespace() const;
    bool isValidMethodReturn() const;
    bool isMethodReturnTypeIdentifier() const;

    void parseIdentifier();
    void parseArguments();
    void parseMethodReturn();
    void parseBodyStart();
    void parseBody();
    void parseItem();
    void setMethodReturnType();

    Compiler *compiler;
    mpc_ast_t *tree;
    mpc_ast_t *item;
    std::vector<Statement*> ret;

    bool waitName;
    bool waitBody;

    PureMethod *method;
};

}
