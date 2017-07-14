#pragma once

#include <3pp/mpc/mpc.h>
#include "namespacedef.hh"
#include "baseparser.hh"
#include "compiler.hh"
#include "puremethod.hh"

namespace nolang {

class NamespaceDefParser : public BaseParser
{
public:
    NamespaceDefParser(mpc_ast_t *);
    NamespaceDef *parse();

private:
    void reset();

    void parseIdentifier();
    void parseCast();
    void parseItem();

    void destroyResult();
    void setValues();

    bool cast;

    NamespaceDef *namespacedef;
    std::vector<std::string> definitions;
};

}
