#pragma once

#include "codegen.hh"
#include "statement.hh"

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

    std::string generateUnit(const Compiler *c);

protected:
    std::string solveNativeType(const TypeDef *t) const;
    std::string generateStatement(Statement *s) const;
    std::string generateBlock(std::vector<std::vector<Statement *>>, std::string ret) const;
};

}
