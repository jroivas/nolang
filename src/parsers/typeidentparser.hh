#pragma once

#include <3pp/mpc/mpc.h>
#include "typeident.hh"
#include "compiler.hh"
#include "puremethod.hh"

namespace nolang {

class TypeIdentParser
{
public:
    TypeIdentParser(mpc_ast_t *);
    TypeIdent *parse();

private:
    void reset();
    void foundColon();

    bool isIdentifierName() const;
    bool isIdentifier() const;
    bool isColon() const;

    void parseIdentifier();
    void parseItem();

    mpc_ast_t *tree;
    mpc_ast_t *item;

    bool wait_colon;
    std::string name;
    std::string type;
};

}
