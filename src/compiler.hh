#pragma once

#include <map>
#include <string>
#include <3pp/mpc/mpc.h>

#include "statement.hh"
#include "puremethod.hh"
#include "methodcall.hh"
#include "namespacedef.hh"
#include "assignment.hh"
#include "typeident.hh"
#include "struct.hh"
#include "const.hh"

namespace nolang
{

class Compiler
{
public:
    Compiler();

    std::vector<Statement*> codegen(mpc_ast_t *tree, PureMethod *m=nullptr, int level=0, bool parameters=false);
    //void parseMethod(mpc_ast_t *tree, int level=0);

    MethodCall *parseMethodCall(mpc_ast_t *);
    void addImport(mpc_ast_t *);
    void addConst(mpc_ast_t *);

    const std::vector<Import*> imports() const
    {
        return m_imports;
    }
    std::map<std::string, PureMethod*> methods() const
    {
        return m_methods;
    }
    const std::vector<Const*> consts() const
    {
        return m_consts;
    }
    const std::vector<Struct*> structs() const
    {
        return m_structs;
    }

    //void parseParamDef(mpc_ast_t *tree, PureMethod *m, int level=0);
    //void parseArgs(mpc_ast_t *tree, PureMethod *m, int level=0);
    void parseMethodRet(mpc_ast_t *tree, PureMethod *m, int level=0);
    void parseStruct(mpc_ast_t *tree);

    std::vector<std::vector<Statement*>> blocks()
    {
        return m_blocks;
    }

    void clearBlocks()
    {
        m_blocks.clear();
    }

protected:
    Import *addImportAs(mpc_ast_t *);
    Import *addImportIdentifierSub(Import *, const std::string &);
    Import *addImportIdentifierAs(Import *, const std::string &);

    void addConstAssignment(mpc_ast_t *item);
    std::vector<Statement*> codegenRecurse(mpc_ast_t *tree, PureMethod *m, int level);

    std::map<std::string, PureMethod*> m_methods;
    std::vector<std::vector<Statement*>> m_blocks;
    std::vector<Import*> m_imports;
    std::vector<Const*> m_consts;
    std::vector<Struct*> m_structs;

    std::string m_last_indent;
};

}
