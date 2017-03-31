#pragma once

#include "codegen.hh"

namespace nolang
{

/**
 * Code generator for C
 * Generates code that can be transpiled.
 */
class Cgen : public CodeGen
{
public:
    Cgen() {}

    std::string generateConst(Statement *);
    std::string generateImport(std::string);
    std::string generateMethod(PureMethod *);
};

}
