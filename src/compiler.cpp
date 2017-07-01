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

std::vector<Statement*> Compiler::appendStatement(std::vector<Statement*> rdata3, Statement *s)
{
    rdata3.push_back(s);
    return rdata3;
}

std::vector<Statement*> Compiler::appendBlock(std::vector<Statement*> rdata3)
{
    parent->m_blocks.push_back(rdata3);
    return std::vector<Statement*>();
}

std::vector<Statement*> Compiler::codegenRecurse(int lvl)
{
    std::vector<Statement*> rdata2;
    iterateTree(tree, [&] (mpc_ast_t *sub) {
        std::vector<Statement*> st = codegen(sub, method, lvl, parameters);
        for (auto s : st) {
            rdata2 = appendStatement(rdata2, s);
            if (isEOS(s)) rdata2 = appendBlock(rdata2);
        }
    });
    return rdata2;
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

void Compiler::parseIndent()
{
    int new_level = std::string(item->contents).length();
    if (!method || new_level == level) return;
    if (parent->m_blocks.empty()) return;

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

Compiler::Compiler()
{
    parent = this;
}

Compiler::Compiler(Compiler *pt, mpc_ast_t *t, PureMethod *m, int l, bool p) :
    tree(t),
    method(m),
    level(l),
    parameters(p)
{
    Compiler *cp = pt;
    while (cp->parent != cp) {
        cp = cp->parent;
    }
    parent = cp;
    item = tree;
}

std::vector<Statement*> Compiler::gen()
{
    item = tree;
    std::string cnts = tree->contents;
    if (isRoot());
    else if (isComment()) recurse = false;
    else if (isMethodDef()) parseMethodDef();
    else if (isStruct()) parseStruct();
    else if (isIndent()) parseIndent();
    else if (isMethodCall()) parseMethodCall();
    else if (isAssignment()) parseAssignment();
    else if (isNumber()) parseNumber();
    else if (isTermOp()) parseTermOp();
    else if (expect(tree, "factorop")) {
        rdata.push_back(new Op(cnts));
    } else if (expect(tree, "string")) {
        rdata.push_back(new StringValue(cnts));
    } else if (expect(tree, "typeident")) {
        TypeIdent *ident = TypeIdentParser(tree).parse();
        if (parameters) {
            rdata.push_back(ident);
        } else if (method != nullptr) {
            method->addVariable(ident);
        } else {
            rdata.push_back(ident);
        }
        recurse = false;
    } else if (expect(tree, "namespacedef")) {
        if (cnts == "false" || cnts == "true") {
            rdata.push_back(new Boolean(cnts));
        } else {
            NamespaceDef *def = NamespaceDefParser(tree).parse();
            if (def != nullptr) {
                rdata.push_back(def);
            } else if (!cnts.empty()) {
                rdata.push_back(new Identifier(cnts));
            }
        }
        recurse = false;
    } else if (expect(tree, "identifier")) {
        // FIXME Some identifiers are special/reserved words
        if (cnts == "false" || cnts == "true") {
            rdata.push_back(new Boolean(cnts));
        } else {
            rdata.push_back(new Identifier(cnts));
        }
    } else if (expect(tree, "import")) {
        parent->m_imports.push_back(ImportParser(tree).parse());
        recurse = false;
    } else if (expect(tree, "const")) {
        parent->m_consts.push_back(ConstParser(this, tree).parse());
        recurse = false;
    } else if (expect(tree, "newline")) {
        rdata.push_back(new EOS());
    } else if (expect(tree, "comparator")) {
        rdata.push_back(new Comparator(cnts));
    } else if (expect(tree, "char", "(") || expect(tree, "char", ")")) {
        rdata.push_back(new Braces(cnts));
    } else if (expect(tree, "ows") || expect(tree, "ws")) {
    } else printError("Unknown node in statement", tree);

    if (recurse) {
        Compiler g(parent, item, method, level, parameters);
        //rdata = applyToVector<Statement*>(rdata, codegenRecurse(tree, m, level));
        //rdata = applyToVector<Statement*>(rdata, g.gen());
        rdata = applyToVector<Statement*>(rdata, g.codegenRecurse(level));
        /*
        m_imports = applyToVector<Import*>(m_imports, g.imports());
        m_consts = applyToVector<Const*>(m_consts, g.consts());
        m_structs = applyToVector<Struct*>(m_structs, g.structs());
        for (auto m : g.methods()) {
            m_methods[m.first] = m.second;
        }
        for (auto b : g.blocks()) {
            m_blocks.push_back(b);
        }
        */
    }

    return rdata;
}

std::vector<Statement*> Compiler::codegen(mpc_ast_t *tree, PureMethod *m, int lvl, bool parameters)
{
    Compiler g(parent, tree, m, lvl, parameters);
    std::vector<Statement*> res = g.gen();
    /*
    m_imports = applyToVector<Import*>(m_imports, g.imports());
    m_consts = applyToVector<Const*>(m_consts, g.consts());
    m_structs = applyToVector<Struct*>(m_structs, g.structs());
    for (auto m : g.methods()) {
        m_methods[m.first] = m.second;
    }
    for (auto b : g.blocks()) {
        m_blocks.push_back(b);
    }
    */
    return res;
    /*
    level = lvl;
    method = m;

    std::vector<Statement*> rdata;
    std::string tag = tree->tag;
    std::string cnts = tree->contents;
    item = tree;
    recurse = true;

    if (isRoot()) ;
    else if (isComment()) recurse = false;
    else if (isMethodDef()) parseMethodDef();
    else if (isStruct()) parseStruct();
    else if (isIndent()) parseIndent();
    else if (isMethodCall()) rdata = parseMethodCall(rdata);
    else if (isAssignment()) rdata = parseAssignment(rdata);
    else if (isNumber()) rdata = parseNumber(rdata);
    else if (isTermOp()) rdata = parseTermOp(rdata);
    else if (expect(tree, "factorop")) {
        rdata.push_back(new Op(cnts));
    } else if (expect(tree, "string")) {
        rdata.push_back(new StringValue(cnts));
    } else if (expect(tree, "typeident")) {
        TypeIdent *ident = TypeIdentParser(tree).parse();
        if (parameters) {
            rdata.push_back(ident);
        } else if (m != nullptr) {
            m->addVariable(ident);
        } else {
            rdata.push_back(ident);
        }
        recurse = false;
    } else if (expect(tree, "namespacedef")) {
        if (cnts == "false" || cnts == "true") {
            rdata.push_back(new Boolean(cnts));
        } else {
            NamespaceDef *def = NamespaceDefParser(tree).parse();
            if (def != nullptr) {
                rdata.push_back(def);
            } else if (!cnts.empty()) {
                rdata.push_back(new Identifier(cnts));
            }
        }
        recurse = false;
    } else if (expect(tree, "identifier")) {
        // FIXME Some identifiers are special/reserved words
        if (cnts == "false" || cnts == "true") {
            rdata.push_back(new Boolean(cnts));
        } else {
            rdata.push_back(new Identifier(cnts));
        }
    } else if (expect(tree, "import")) {
        m_imports.push_back(ImportParser(tree).parse());
        recurse = false;
    } else if (expect(tree, "const")) {
        m_consts.push_back(ConstParser(this, tree).parse());
        recurse = false;
    } else if (expect(tree, "newline")) {
        rdata.push_back(new EOS());
    } else if (expect(tree, "comparator")) {
        rdata.push_back(new Comparator(cnts));
    } else if (expect(tree, "char", "(") || expect(tree, "char", ")")) {
        rdata.push_back(new Braces(cnts));
    } else if (expect(tree, "ows") || expect(tree, "ws")) {
    } else printError("Unknown node in statement", tree);

    if (recurse) {
        Compiler g(tree, method, level);
        rdata = applyToVector<Statement*>(rdata, g.gen());
        //rdata = applyToVector<Statement*>(rdata, codegenRecurse(tree, m, level));
    }

    return rdata;
    */
}
