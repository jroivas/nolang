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
    Cgen();

    std::string generateConst(const Statement *);
    std::string generateImport(const std::string &);
    std::string generateMethod(const PureMethod *);

    std::string generateUnit(const Compiler *c);

protected:
    std::string solveNativeType(const Statement *t) const;
    std::string generateStatement(const Statement *s);
    std::string generateBlock(const std::vector<std::vector<Statement *>> &, const std::string &ret);

    std::string generateMethodCall(const MethodCall *m);
    std::string solveTypeOfChain(std::vector<Statement*>) const;

    std::string autogen();

private:
    unsigned int m_autogen_index;
    std::string m_autogen_prefix;
};

}
