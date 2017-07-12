#pragma once

#include <3pp/mpc/mpc.h>
#include "baseparser.hh"
#include "import.hh"

namespace nolang {

class ImportParser : public BaseParser
{
public:
    ImportParser(mpc_ast_t *);
    Import *parse();

private:
    bool isImport() const;
    bool isImportString() const;
    bool isSub() const { return isNamespaceDef(); }

    void reset();
    void parseItem();
    void createImport();
    void addAs();
    void addSub();
    void parseSub();
    void parseSubIdentifier();
    void parseAs();

    mpc_ast_t *tree;

    Import *res;
};

}
