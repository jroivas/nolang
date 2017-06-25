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
    bool isAs();

    void reset();
    void parseItem();
    void createImport();
    void addAs();
    void parseAs();

    mpc_ast_t *tree;
    mpc_ast_t *item;

    Import *res;
};

}
