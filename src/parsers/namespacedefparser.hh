#pragma once

#include <3pp/mpc/mpc.h>
#include "namespacedef.hh"
#include "compiler.hh"
#include "puremethod.hh"

namespace nolang {

class NamespaceDefParser
{
public:
    NamespaceDefParser(mpc_ast_t *);
    NamespaceDef *parse();

private:
    void reset();

    bool isIdentifier() const;
    bool isCast() const;
    bool isDot() const;

    void parseIdentifier();
    void parseCast();
    void parseItem();

    void destroyResult();
    void setValues();

    mpc_ast_t *tree;
    mpc_ast_t *item;
    bool cast;

    NamespaceDef *namespacedef;
    std::vector<std::string> definitions;
};

}
