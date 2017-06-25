#include "compiler.hh"
#include "tools.hh"
#include "parsers/assignmentparser.hh"
#include "parsers/constparser.hh"
#include "parsers/methodparser.hh"
#include "parsers/methodcallparser.hh"
#include "parsers/namespacedefparser.hh"
#include "parsers/structparser.hh"
#include "parsers/typeidentparser.hh"

#include <iostream>

using namespace nolang;

Compiler::Compiler()
{
}

Import *Compiler::addImportIdentifierSub(Import *imp, const std::string &cnts)
{
    if (imp == nullptr) imp = new Import(cnts);
    else imp->addSub(cnts);
    return imp;
}

Import *Compiler::addImportIdentifierAs(Import *imp, const std::string &cnts)
{
    if (imp == nullptr) imp = new Import(cnts);
    else imp->addAs(cnts);
    return imp;
}

Import *Compiler::addImportAs(mpc_ast_t *tree)
{
    Import *imp = nullptr;

    iterateTree(tree, [&] (mpc_ast_t *item) {
        if (expect(item, "identifier"))
            imp = addImportIdentifierSub(imp, item->contents);
    });

    return imp;
}

void Compiler::addImport(mpc_ast_t *tree)
{
    Import *imp = nullptr;

    iterateTree(tree, [&] (mpc_ast_t *item) {
        if (expect(item, "identifier"))
            imp = addImportIdentifierAs(imp, item->contents);
        else if (expect(item, "namespacedef"))
            imp = addImportAs(item);
    });
    if (imp) m_imports.push_back(imp);
}

std::vector<Statement*> Compiler::codegenRecurse(mpc_ast_t *tree, PureMethod *m, int level)
{
    std::vector<Statement*> rdata;
    iterateTree(tree, [&] (mpc_ast_t *item) {
        std::vector<Statement*> st = codegen(item, m, level);
        for (auto s : st) {
            if (s->type() == "EOS") {
                rdata.push_back(s);
                m_blocks.push_back(rdata);
                rdata = std::vector<Statement*>();
            } else {
                rdata.push_back(s);
            }
        }
    });
    return rdata;
}

std::vector<Statement*> Compiler::codegen(mpc_ast_t *tree, PureMethod *m, int level, bool parameters)
{
    std::vector<Statement*> rdata;

    std::string tag = tree->tag;
    std::string cnts = tree->contents;
    bool recurse = true;

    if (tag == ">") {
        //std::cout << "ROOT\n";
    } else if (expect(tree, "comment")) {
        // Ignore comments
        recurse = false;
    } else if (expect(tree, "methoddef")) {
        // New method
        PureMethod *method = MethodParser(this, tree).parse();
        if (method == nullptr) throw "Invalid method!";
        m_methods[method->name()] = method;
        recurse = false;
        level = 0;
    } else if (expect(tree, "struct")) {
        m_structs.push_back(StructParser(tree).parse());
        recurse = false;
    } else if (expect(tree, "indent")) {
        int new_level = cnts.length();
        if (new_level != level) {
            if (m && !m_blocks.empty()) {
                m->addBlock(m_blocks);
                m_blocks = std::vector<std::vector<Statement*>>();
            }
        }
        // SKIP and recurse
    } else if (expect(tree, "methodcall")) {
        rdata.push_back(MethodCallParser(this, tree).parse());
        recurse = false;
    } else if (expect(tree, "assignment")) {
        rdata.push_back(AssignmentParser(this, tree, m).parse());
        recurse = false;
    } else if (expect(tree, "number")) {
        rdata.push_back(new NumberValue(cnts));
    } else if (expect(tree, "termop")) {
        rdata.push_back(new Op(cnts));
    } else if (expect(tree, "factorop")) {
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
        addImport(tree);
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

    if (recurse)
        rdata = applyToVector<Statement*>(rdata, codegenRecurse(tree, m, level));

    return rdata;
}
