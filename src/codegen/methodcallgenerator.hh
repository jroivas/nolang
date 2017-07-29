#pragma once

#include <map>
#include <vector>

#include "codegen.hh"
#include "moduledef.hh"
#include "statement.hh"

namespace nolang
{

class Cgen;

class MethodCallGenerator
{
public:
    MethodCallGenerator(Cgen *, const MethodCall *, const PureMethod *);

    void generateParameterStatements();
    bool isStruct();

    std::string getModuleName() const;
    std::vector<std::string> generateStructInitCall();
    std::vector<std::string> generateMethodCall();


protected:
    void solveParameterNames();
    void solveParameterTypes();
    void solveParameterNolangTypes();
    void getNamespaceDef();
    std::string generateBuiltInIOPrint() const;
    std::string generateLocalMethodCall() const;
    std::vector<std::string> generateStructInitStatements();
    const ModuleMethodDef *getModuleMethodDef(const ModuleDef *mod) const;
    std::vector<std::string> generateModuleMethodCall(const ModuleDef *mod);
    std::vector<std::string> generateModuleMethodCallWithMethod(const ModuleMethodDef *meth) const;


private:
    Cgen *cgen;
    const MethodCall *mc;
    const PureMethod *m;
    const NamespaceDef *def;
    const ModuleDef *mod;

    std::vector<std::string> pnames;
    std::vector<std::string> ptypes;
    std::vector<std::string> nolang_ptypes;
    std::vector<std::string> parameter_statements;
};

}
