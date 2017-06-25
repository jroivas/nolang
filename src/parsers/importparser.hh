#pragma once

#include <3pp/mpc/mpc.h>
#include "import.hh"

namespace nolang {

class ImportParser
{
public:
    ImportParser(mpc_ast_t *);
    Import *parse();

private:
    bool isImport() const;
    bool isIdentifier() const;
    bool isSub() const;

    void reset();
    void parseItem();
    void createImport();
    void addAs();
    void addSub();
    void parseSub();
    void parseSubIdentifier();
    void parseAs();

    mpc_ast_t *tree;
    mpc_ast_t *item;

    Import *res;
};

}
