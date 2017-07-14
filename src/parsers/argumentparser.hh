#pragma once

#include <3pp/mpc/mpc.h>
#include <vector>
#include "baseparser.hh"
#include "puremethod.hh"
#include "compiler.hh"

namespace nolang {

class ArgumentParser : public BaseParser<void>
{
public:
    ArgumentParser(Compiler *, mpc_ast_t *);
    std::vector<TypeIdent*> parseList();

private:
    void reset();

    bool isOpenBrace() const;
    bool isCloseBrace() const;
    bool isParamDef() const;
    bool isTypeIdent() const;
    bool isComma() const;
    bool isTypeIdentifier(Statement *s) const;

    void parseTypeIdent();
    void parseParamDef();
    void parseItem();

    Compiler *compiler;

    int braceOpenLevel;

    std::vector<TypeIdent*> params;
};

}

