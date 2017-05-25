#pragma once

#include <map>
#include <vector>

#include "codegen.hh"
#include "moduledef.hh"
#include "statement.hh"

namespace nolang
{

/**
 * Code generator for C
 * Generates transpiled C code from parsed/compiled Nolang.
 */
class Cgen : public CodeGen
{
public:
    Cgen();

    std::string generateConst(const Statement *);
    std::string generateImport(const Import *);
    std::string generateMethod(const PureMethod *);
    std::string generateMethodPrototype(const PureMethod *);

    std::string generateUnit(const Compiler *c);

protected:
    std::string solveNativeType(const Statement *t, const PureMethod *m) const;
    std::string solveNativeType(const std::string & s) const;
    std::string solveReturnType(const Statement *t, const PureMethod *m) const;
    std::vector<std::string> generateStatement(const Statement *s, const PureMethod *m);
    std::vector<std::string> generateStatements(const std::vector<Statement *> stmts, const PureMethod *m);
    std::vector<std::string> generateBlock(const std::vector<std::vector<Statement *>> &, const std::string &ret, const PureMethod *m);
    std::vector<std::string> generateVariable(const TypeIdent *i);
    TypeIdent *solveVariable(const std::string &name, const PureMethod *m) const;

    std::vector<std::string> generateMethodCall(const MethodCall *mc, const PureMethod *m);
    std::string solveTypeOfChain(std::vector<Statement*>, const PureMethod *m) const;

    std::string autogen();

    ModuleDef *getModule(std::string name);

private:
    unsigned int m_autogen_index;
    std::string m_autogen_prefix;
    std::map<std::string, ModuleDef*> m_modules;
    ModuleDef *m_current_module;
};

}
