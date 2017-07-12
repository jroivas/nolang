#pragma once

#include <3pp/mpc/mpc.h>
#include "baseparser.hh"
#include "struct.hh"

namespace nolang {

class StructParser : public BaseParser
{
public:
    StructParser(mpc_ast_t *);
    Struct *parse();

private:
    bool isStruct();
    bool isData();

    void reset();
    void parseItem();
    void parseData();
    void createStruct();

    mpc_ast_t *tree;

    Struct *res;
};

}
