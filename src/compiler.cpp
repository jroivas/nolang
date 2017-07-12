#include "compiler.hh"
#include "parsers/assignmentparser.hh"
#include "parsers/constparser.hh"
#include "parsers/importparser.hh"
#include "parsers/methodparser.hh"
#include "parsers/methodcallparser.hh"
#include "parsers/namespacedefparser.hh"
#include "parsers/numberparser.hh"
#include "parsers/structparser.hh"
#include "parsers/typeidentparser.hh"

#include <iostream>

using namespace nolang;

Compiler::Compiler() :
    recurse(true)
{
    parent = this;
}

Compiler::Compiler(Compiler *pt, mpc_ast_t *t, PureMethod *m, int l, bool p) :
    tree(t),
    method(m),
    level(l),
    parameters(p),
    recurse(true)
{
    Compiler *cp = pt;
    while (cp->parent != cp) cp = cp->parent;
    parent = cp;
    recurse = true;
}


void Compiler::appendStatement(Statement *s)
{
    rdata.push_back(s);
}

void Compiler::appendBlock()
{
    parent->m_blocks.push_back(rdata);
    rdata = std::vector<Statement*>();
}

std::vector<Statement*> Compiler::codegenRecurse(int lvl)
{
    iterateTree(tree, [&] (mpc_ast_t *sub) {
        auto st = codegen(sub, method, lvl, parameters);
        for (auto s : st) {
            appendStatement(s);
            if (isEOS(s)) appendBlock();
        }
    });
    return rdata;
}

void Compiler::parseMethodDef()
{
    PureMethod *new_method = MethodParser(this, item).parse();
    if (new_method == nullptr) throw "Invalid method!";
    parent->m_methods[new_method->name()] = new_method;
    recurse = false;
    level = 0;
}

void Compiler::parseStruct()
{
    parent->m_structs.push_back(StructParser(item).parse());
    recurse = false;
}

bool Compiler::isValidIndentBlock()
{
    int new_level = std::string(item->contents).length();
    return method && new_level != level && !parent->m_blocks.empty();
}

void Compiler::parseIndent()
{
    if (!isValidIndentBlock()) return;

    method->addBlock(parent->m_blocks);
    parent->m_blocks = std::vector<std::vector<Statement*>>();
}

void Compiler::parseMethodCall()
{
    rdata.push_back(MethodCallParser(this, item).parse());
    recurse = false;
}

void Compiler::parseAssignment()
{
    rdata.push_back(AssignmentParser(this, item, method).parse());
    recurse = false;
}

void Compiler::parseNumber()
{
    rdata.push_back(NumberParser(item).parse());
}

void Compiler::parseTermOp()
{
    rdata.push_back(new Op(item->contents));
}

void Compiler::parseFactorOp()
{
    rdata.push_back(new Op(item->contents));
}

void Compiler::parseString()
{
    rdata.push_back(new StringValue(item->contents));
}

void Compiler::parseTypeIdent()
{
    TypeIdent *ident = TypeIdentParser(item).parse();
    if (parameters) rdata.push_back(ident);
    else if (method != nullptr) method->addVariable(ident);
    else rdata.push_back(ident);
    recurse = false;
}

void Compiler::parseNamespace()
{
    std::string cnts = item->contents;
    NamespaceDef *def = NamespaceDefParser(item).parse();
    if (def != nullptr) rdata.push_back(def);
    else if (!cnts.empty()) rdata.push_back(new Identifier(cnts));
}

void Compiler::parseBoolean()
{
    rdata.push_back(new Boolean(item->contents));
}

void Compiler::parseNamespaceDef()
{
    if (isBooleanDef()) parseBoolean();
    else parseNamespace();
    recurse = false;
}

void Compiler::parseIdentifier()
{
    // FIXME Some identifiers are special/reserved words
    if (isBooleanDef()) parseBoolean();
    else rdata.push_back(new Identifier(item->contents));
}

void Compiler::parseImport()
{
    parent->m_imports.push_back(ImportParser(item).parse());
    recurse = false;
}

void Compiler::parseConst()
{
    parent->m_consts.push_back(ConstParser(this, item).parse());
    recurse = false;
}

void Compiler::parseNewLine()
{
    rdata.push_back(new EOS());
}

void Compiler::parseComparator()
{
    rdata.push_back(new Comparator(item->contents));
}

void Compiler::parseBrace()
{
    rdata.push_back(new Braces(item->contents));
}

void Compiler::doRecurse()
{
    if (!recurse) return;
    Compiler g(parent, item, method, level, parameters);
    rdata = applyToVector<Statement*>(rdata, g.codegenRecurse(level));
}

std::vector<Statement*> Compiler::gen()
{
    item = tree;
    if (isRoot() || isBody());
    else if (isComment()) recurse = false;
    else if (isMethodDef()) parseMethodDef();
    else if (isStruct()) parseStruct();
    else if (isIndent()) parseIndent();
    else if (isMethodCall()) parseMethodCall();
    else if (isAssignment()) parseAssignment();
    else if (isNumber()) parseNumber();
    else if (isTermOp()) parseTermOp();
    else if (isFactorOp()) parseFactorOp();
    else if (isString()) parseString();
    else if (isTypeIdent()) parseTypeIdent();
    else if (isNamespaceDef()) parseNamespaceDef();
    else if (isIdentifier()) parseIdentifier();
    else if (isImport()) parseImport();
    else if (isConst()) parseConst();
    else if (isNewLine()) parseNewLine();
    else if (isComparator()) parseComparator();
    else if (isBrace()) parseBrace();
    else if (isWhitespace());
    else printError("Unknown node in statement", tree);

    doRecurse();

    return rdata;
}

std::vector<Statement*> Compiler::codegen(mpc_ast_t *tree, PureMethod *m, int lvl, bool parameters)
{
    return Compiler(parent, tree, m, lvl, parameters).gen();
}
