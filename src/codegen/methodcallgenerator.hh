#pragma once

#include <map>
#include <vector>

#include "codegen.hh"
#include "moduledef.hh"
#include "statement.hh"

namespace nolang
{

class MethodCallGenerator
{
public:
    MethodCallGenerator(const MethodCall *, const PureMethod *);

    void generateParameterStatements();
    bool isStruct();

    std::string getModuleName() const;
    std::vector<std::string> generateStructInitCall(std::string postponed);
    std::vector<std::string> generateMethodCall(const ModuleDef *mod, std::string postponed);


protected:
    void solveParameterNames();
    void solveParameterTypes();
    void solveParameterNolangTypes();
    void getNamespaceDef();
    std::string generateBuiltInIOPrint() const;
    std::string generateLocalMethodCall() const;
    std::vector<std::string> generateStructInitStatements(std::string postponed);
    const ModuleMethodDef *getModuleMethodDef(const ModuleDef *mod) const;
    std::vector<std::string> generateModuleMethodCall(const ModuleDef *mod);
    std::vector<std::string> generateModuleMethodCallWithMethod(const ModuleMethodDef *meth) const;


private:
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
