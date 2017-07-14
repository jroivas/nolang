#pragma once

#include <3pp/mpc/mpc.h>
#include "baseparser.hh"
#include "struct.hh"

namespace nolang {

class StructParser : public BaseParser<Struct>
{
public:
    StructParser(mpc_ast_t *);

private:
    bool isStruct();
    bool isStructString() { return expect(item, "string", "struct"); }
    bool isBraceOpen() { return expect(item, "char", "{"); }
    bool isBraceClose() { return expect(item, "char", "}"); }
    bool isData();

    void reset();
    void parseItem();
    void parseData();
    void createStruct();

    bool got_struct;
    bool begin_struct;
    bool end_struct;
};

}
