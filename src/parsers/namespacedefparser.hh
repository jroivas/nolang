#pragma once

#include <3pp/mpc/mpc.h>
#include "namespacedef.hh"
#include "baseparser.hh"
#include "compiler.hh"
#include "puremethod.hh"

namespace nolang {

class NamespaceDefParser : public BaseParser<NamespaceDef>
{
public:
    NamespaceDefParser(mpc_ast_t *);

private:
    void reset();
    void postProcess();

    void parseIdentifier();
    void parseCast();
    void parseItem();

    void destroyResult();
    void setValues();

    bool cast;

    std::vector<std::string> definitions;
};

}
