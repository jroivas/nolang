#pragma once

#include <3pp/mpc/mpc.h>
#include "baseparser.hh"
#include "methodcall.hh"
#include "compiler.hh"

namespace nolang {

class MethodCallParser : public BaseParser<MethodCall>
{
public:
    MethodCallParser(Compiler *, mpc_ast_t *);

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

    bool wait_ns;
    bool wait_call_end;
};

}
