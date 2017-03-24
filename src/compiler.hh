#pragma once

#include <map>
#include <string>
#include <3pp/mpc/mpc.h>

#include "puremethod.hh"

namespace nolang
{

class Compiler
{
public:
    Compiler() {}

    std::string codegen(mpc_ast_t *tree, PureMethod *m=nullptr, int level=0);
    void parseMethod(mpc_ast_t *tree, int level=0);

    void addImport(mpc_ast_t *tree);
    void dump() const;

protected:

    std::map<std::string, PureMethod*> m_methods;
    std::vector<std::string> m_blocks;
    std::vector<std::string> m_imports;
    std::vector<std::string> m_consts;
};

}
