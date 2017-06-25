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
#include "import.hh"

namespace nolang
{

class Compiler
{
public:
    Compiler();

    std::vector<Statement*> codegen(mpc_ast_t *tree, PureMethod *m=nullptr, int level=0, bool parameters=false);

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

    std::vector<std::vector<Statement*>> blocks()
    {
        return m_blocks;
    }

    void clearBlocks()
    {
        m_blocks.clear();
    }

protected:
    std::vector<Statement*> codegenRecurse(mpc_ast_t *tree, PureMethod *m, int level);
    NumberValue *parseNumber(mpc_ast_t *tree);

    std::map<std::string, PureMethod*> m_methods;
    std::vector<std::vector<Statement*>> m_blocks;
    std::vector<Import*> m_imports;
    std::vector<Const*> m_consts;
    std::vector<Struct*> m_structs;

    std::string m_last_indent;
};

}
