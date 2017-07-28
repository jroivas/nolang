#pragma once

#include <map>
#include <vector>

#include "codegen.hh"
#include "moduledef.hh"
#include "statement.hh"
#include "namespacedef.hh"
#include "assignment.hh"
#include "typeident.hh"
#include "struct.hh"
#include "const.hh"

#include "methodcallgenerator.hh"

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
    static std::string autogen();
    std::string solveTypeOfChain(std::vector<Statement*>, const PureMethod *m) const;
    std::string solveNolangTypeOfChain(std::vector<Statement*> chain, const PureMethod *m) const;
    std::vector<std::string> generateStatements(const std::vector<Statement *> stmts, const PureMethod *m);
    bool isStruct(const std::string &name) const;
    const Struct *getStruct(const std::string &name) const;
    const ModuleDef *getModule(std::string name) const;

    std::vector<std::string> applyPostponed(std::vector<std::string> &);

    std::string usePostponedMethod();

protected:
    std::string generateStructInitializer(const Struct *c);
    std::string generateVariableInit(const TypeIdent *i);
    std::string solveNativeType(const Statement *t, const PureMethod *m) const;
    std::string solveNolangType(const Statement *t, const PureMethod *m) const;
    std::string solveNativeType(const std::string & s) const;
    std::string solveReturnType(const Statement *t, const PureMethod *m) const;
    bool isAssignmentMethodCall(const Assignment *) const;
    bool isNativeType(const std::string & s) const;
    std::vector<std::string> generateStatement(const Statement *s, const PureMethod *m);
    std::vector<std::string> generateBlock(const std::vector<std::vector<Statement *>> &, const std::string &ret, const PureMethod *m);
    std::vector<std::string> generateVariable(const TypeIdent *i) const;
    TypeIdent *solveVariable(const std::string &name, const PureMethod *m) const;

    std::vector<std::string> generateMethodCall(const MethodCall *mc, const PureMethod *m);
    std::string castCode(const std::string &src_var, const std::string &src_type, const std::string &to_type) const;

private:
    const std::string indent() const { return "    "; }
    bool isBuiltInImport(std::string n) const  { return n == "IO"; }
    bool isValidImport(const ModuleDef *m) const { return m == nullptr; }
    bool isValidModuleDef(const ModuleDef *m) const { return m->ok(); }

    std::string generateIOImport() const;
    std::string generateModuleInitCode(std::string, const ModuleDef *);
    std::string generateModuleFixme(const Import *, std::string, ModuleDef *def) const;
    std::string generateModuleImport(const Import *, std::string);

    std::string generateHeaders();
    std::string generateImports(const Compiler *);
    std::string generateStructs(const Compiler *);
    std::string generateInitializers(const Compiler *);
    std::string generateConsts(const Compiler *);
    std::string generatePrototypes(const Compiler *);
    std::string generateMethods(const Compiler *);

    std::string generateStructHeader(const Struct *) const;
    std::string generateStructFooter(const Struct *) const;
    std::string generateStructElements(const Struct *) const;
    void addStruct(const Struct *);

    std::string generateStructInitPrototype(const Struct *) const;
    std::string generateStructAlloc(const Struct *) const;
    std::string generateStructElementInit(const Struct *) const;
    std::string generateStructElementInitCall(const TypeIdent *i) const;

    bool isValidConst(const Const *) const;
    std::string generateConstPrototype(const Const *) const;
    std::string generateConstAssignment(const Const *);

    void evaluatePendingAssignment();
    std::map<std::string, const ModuleDef*> m_modules;
    const ModuleDef *m_current_module;
    std::string m_postponed_assignment;
    std::string m_postponed_method;
    std::vector<const Struct*> m_structs;
};

}
