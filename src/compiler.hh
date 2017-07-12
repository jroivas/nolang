#pragma once

#include <map>
#include <string>
#include <iostream>
#include <3pp/mpc/mpc.h>

#include "parsers/baseparser.hh"
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

class Compiler : public BaseParser
{
public:
    Compiler();
    Statement *parse() { throw std::string("Not implemented"); }

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

    void appendStatement(Statement *s);
    void appendBlock();

    std::vector<Statement*> codegenRecurse(int level);

    std::map<std::string, PureMethod*> m_methods;
    std::vector<std::vector<Statement*>> m_blocks;
    std::vector<Import*> m_imports;
    std::vector<Const*> m_consts;
    std::vector<Struct*> m_structs;

    bool isEOS(Statement *s) const { return s->type() == "EOS"; }
    bool isBooleanDef() const {
        std::string cnts = item->contents;
        return cnts == "false" || cnts == "true";
    }
    bool isValidIndentBlock();

    void parseMethodDef();
    void parseMethodCall();
    void parseStruct();
    void parseIndent();
    void parseAssignment();
    void parseNumber();
    void parseTermOp();
    void parseFactorOp();
    void parseString();
    void parseTypeIdent();
    void parseNamespace();
    void parseNamespaceDef();
    void parseIdentifier();
    void parseImport();
    void parseConst();
    void parseNewLine();
    void parseComparator();
    void parseBrace();
    void parseBoolean();
    void doRecurse();

    Compiler *parent;
    mpc_ast_t *tree;
    PureMethod *method;

    int level;
    bool parameters;
    bool recurse;

    std::vector<Statement*> rdata;
};

}
