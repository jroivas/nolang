#pragma once

#include <3pp/mpc/mpc.h>
#include <vector>
#include "puremethod.hh"
#include "compiler.hh"

namespace nolang {

class ArgumentParser
{
public:
    ArgumentParser(Compiler *, mpc_ast_t *);
    std::vector<TypeIdent*> parse();

private:
    void reset();

    bool isOpenBrace() const;
    bool isCloseBrace() const;
    bool isParamDef() const;
    bool isTypeIdent() const;
    bool isComma() const;
    bool isTypeIdentifier(Statement *s) const;
    bool isOptionalWhitespace() const;

    void parseTypeIdent();
    void parseParamDef();
    void parseItem();

    Compiler *compiler;
    mpc_ast_t *tree;
    mpc_ast_t *item;

    int braceOpenLevel;

    std::vector<TypeIdent*> params;
};

}

