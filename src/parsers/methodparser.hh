#pragma once

#include <3pp/mpc/mpc.h>
#include "puremethod.hh"
#include "baseparser.hh"
#include "compiler.hh"

namespace nolang {

class MethodParser : public BaseParser
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
    bool isBodyStart() const;
    bool isBody() const;
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
    std::vector<Statement*> ret;

    bool waitName;
    bool waitBody;

    PureMethod *method;
};

}
