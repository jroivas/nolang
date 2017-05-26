#pragma once

#include <map>
#include <string>
#include <3pp/mpc/mpc.h>

#include "statement.hh"
#include "puremethod.hh"
#include "methodcall.hh"

namespace nolang
{

class Compiler
{
public:
    Compiler();

    //std::string codegen(mpc_ast_t *tree, PureMethod *m=nullptr, int level=0);
    std::vector<Statement*> codegen(mpc_ast_t *tree, PureMethod *m=nullptr, int level=0);
    void parseMethod(mpc_ast_t *tree, int level=0);

    MethodCall *parseMethodCall(mpc_ast_t *);
    NamespaceDef *parseNamespaceDef(mpc_ast_t *);
    void addImport(mpc_ast_t *);
    void addConst(mpc_ast_t *);

    void dump() const;
    void dumpStatement(Statement *s, int level=0) const;

    std::vector<Import*> imports() const
    {
        return m_imports;
    }
    std::map<std::string, PureMethod*> methods() const
    {
        return m_methods;
    }
    TypeIdent *parseTypeIdent(mpc_ast_t *tree, PureMethod *m, int level=0);
    Assignment *parseAssignment(mpc_ast_t *tree, PureMethod *m, int level=0);

    void parseParamDef(mpc_ast_t *tree, PureMethod *m, int level=0);
    void parseArgs(mpc_ast_t *tree, PureMethod *m, int level=0);
    void parseMethodRet(mpc_ast_t *tree, PureMethod *m, int level=0);

protected:
    bool expect(mpc_ast_t *tree, std::string key, std::string val="") const;
    Import *addImportAs(mpc_ast_t *);

    std::map<std::string, PureMethod*> m_methods;
    std::vector<std::vector<Statement*>> m_blocks;
    //std::vector<std::string> m_blocks;
    //std::vector<std::string> m_imports;
    std::vector<Import*> m_imports;
    std::vector<std::string> m_consts;

    std::string m_last_indent;
    bool m_parameters;
};

}
