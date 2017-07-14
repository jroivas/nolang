#pragma once

#include <3pp/mpc/mpc.h>
#include "baseparser.hh"
#include "typeident.hh"
#include "compiler.hh"
#include "puremethod.hh"

namespace nolang {

class TypeIdentParser : public BaseParser
{
public:
    TypeIdentParser(mpc_ast_t *);
    TypeIdent *parse();

private:
    void reset();
    void foundColon();

    bool isIdentifierName() const;
    bool isColon() const;

    void parseIdentifier();
    void parseItem();

    bool wait_colon;
    std::string name;
    std::string type;
};

}
