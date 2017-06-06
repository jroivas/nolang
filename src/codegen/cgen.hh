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

    std::string generateImport(const Import *);
    std::string generateMethod(const PureMethod *);
    std::string generateMethodPrototype(const PureMethod *);
    std::string generateConst(const Const *);
    std::string generateStruct(const Struct *);

    std::string generateUnit(const Compiler *c);

protected:
    std::string generateStructInitializer(const Struct *c);
    std::string generateVariableInit(const TypeIdent *i);
    std::string solveNativeType(const Statement *t, const PureMethod *m) const;
    std::string solveNolangType(const Statement *t, const PureMethod *m) const;
    std::string solveNativeType(const std::string & s) const;
    std::string solveReturnType(const Statement *t, const PureMethod *m) const;
    bool isNativeType(const std::string & s) const;
    std::vector<std::string> generateStatement(const Statement *s, const PureMethod *m);
    std::vector<std::string> generateStatements(const std::vector<Statement *> stmts, const PureMethod *m);
    std::vector<std::string> generateBlock(const std::vector<std::vector<Statement *>> &, const std::string &ret, const PureMethod *m);
    std::vector<std::string> generateVariable(const TypeIdent *i);
    TypeIdent *solveVariable(const std::string &name, const PureMethod *m) const;

    std::vector<std::string> generateMethodCall(const MethodCall *mc, const PureMethod *m);
    std::string solveTypeOfChain(std::vector<Statement*>, const PureMethod *m) const;
    std::string solveNolangeTypeOfChain(std::vector<Statement*> chain, const PureMethod *m) const;
    std::string castCode(const std::string &src_var, const std::string &src_type, const std::string &to_type) const;

    std::string autogen();

    const ModuleDef *getModule(std::string name) const;
    std::vector<std::string> applyPostponed(std::vector<std::string> &);

private:
    void evaluatePendingAssignment();
    unsigned int m_autogen_index;
    std::string m_autogen_prefix;
    std::map<std::string, ModuleDef*> m_modules;
    const ModuleDef *m_current_module;
    std::string m_postponed_assignment;
};

}
