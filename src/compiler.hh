#pragma once

#include <map>
#include <string>
#include <iostream>
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
#include "tools.hh"

namespace nolang
{

class Compiler
{
public:
    Compiler();

    std::vector<Statement*> codegen(mpc_ast_t *tree, PureMethod *m=nullptr, int level=0, bool parameters=false);

    const std::vector<Import*> imports() const { return parent->m_imports; }
    std::map<std::string, PureMethod*> methods() const { return parent->m_methods; }
    const std::vector<Const*> consts() const { return parent->m_consts; }
    const std::vector<Struct*> structs() const { return parent->m_structs; }
    std::vector<std::vector<Statement*>> blocks() { return parent->m_blocks; }
    void clearBlocks() { parent->m_blocks.clear(); }

protected:
    Compiler(Compiler *, mpc_ast_t *, PureMethod *, int level, bool parameters);
    std::vector<Statement*> gen();

    std::vector<Statement*> appendStatement(std::vector<Statement*>, Statement *s);
    std::vector<Statement*> appendBlock(std::vector<Statement*>);

    std::vector<Statement*> codegenRecurse(int level);

    std::map<std::string, PureMethod*> m_methods;
    std::vector<std::vector<Statement*>> m_blocks;
    std::vector<Import*> m_imports;
    std::vector<Const*> m_consts;
    std::vector<Struct*> m_structs;

    bool isEOS(Statement *s) const { return s->type() == "EOS"; }
    bool isRoot() const { return std::string(item->tag) == ">"; }
    bool isComment() const { return expect(item, "comment"); }
    bool isMethodDef() const { return expect(item, "methoddef"); }
    bool isMethodCall() const { return expect(item, "methodcall"); }
    bool isStruct() const { return expect(item, "struct"); }
    bool isIndent() const { return expect(item, "indent"); }
    bool isAssignment() const { return expect(item, "assignment"); }
    bool isNumber() const { return expect(item, "number"); }
    bool isTermOp() const { return expect(item, "termop"); }

    void parseMethodDef();
    void parseMethodCall();
    void parseStruct();
    void parseIndent();
    void parseAssignment();
    void parseNumber();
    void parseTermOp();

    Compiler *parent;
    mpc_ast_t *tree;
    mpc_ast_t *item;
    PureMethod *method;

    int level;
    bool recurse;
    bool parameters;

    std::vector<Statement*> rdata;
};

}
