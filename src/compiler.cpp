#include "compiler.hh"
#include "tools.hh"
#include "parsers/assignmentparser.hh"

#include <iostream>

using namespace nolang;

Compiler::Compiler() :
    m_parameters(false)
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

void Compiler::addConstAssignment(mpc_ast_t *item)
{
    PureMethod tmp;
    AssignmentParser p(this, item, &tmp);
    Assignment *assignment = p.parse();
    if (assignment && tmp.variables().size() == 1)
        m_consts.push_back(new Const(tmp.variables()[0], assignment));
    else printError("Invalid const", item);
}

void Compiler::addConst(mpc_ast_t *tree)
{
    bool wait_const = true;
    iterateTree(tree, [&] (mpc_ast_t *item) {
        if (expect(item, "string", "const"))
            wait_const = false;
        else if (!wait_const & expect(item, "assignment"))
            addConstAssignment(item);
        else printError("Unknown node in const defination", item);
    });
}

NamespaceDef *Compiler::parseNamespaceDefStrings(mpc_ast_t *tree)
{
    NamespaceDef *def = new NamespaceDef();
    std::vector<std::string> res;
    bool cast = false;

    iterateTree(tree, [&] (mpc_ast_t *item) {
        std::string cnts = item->contents;
        if (expect(item, "identifier")) {
            if (cast) def->setCast(cnts);
            else res.push_back(cnts);
        } else if (cnts == "::") {
            cast = true;
        } else if (cnts == ".") { // FIXME
        } else printError("Unknown node in namespace defination", item);
    });
    if (!res.empty()) def->setValues(res);
    return def;
}

NamespaceDef *Compiler::parseNamespaceDef(mpc_ast_t *tree)
{
    NamespaceDef *res = parseNamespaceDefStrings(tree);
    if (!res->isValid()) {
        delete res;
        return nullptr;
    }
    return res;
}

MethodCall *Compiler::parseMethodCall(mpc_ast_t *tree)
{
    MethodCall *mcall = new MethodCall();

    bool wait_ns = true;
    bool wait_call_end = false;

    iterateTree(tree, [&] (mpc_ast_t *item) {
        if (wait_ns && expect(item, "identifier")) {
            mcall->setNamespace(new NamespaceDef(item->contents));
            wait_ns = false;
        } else if (wait_ns && expect(item, "namespacedef")) {
            mcall->setNamespace(parseNamespaceDef(item));
            wait_ns = false;
        } else if (!wait_call_end && expect(item, "char", "("))
            wait_call_end = true;
        else if (wait_call_end && expect(item, "char", ")"))
            wait_call_end = false;
        else if (wait_call_end)
            mcall->addParameter(codegen(item));
        else printError("Unknown node in method call", item);
    });

    return mcall;
}

TypeIdent *Compiler::parseTypeIdent(mpc_ast_t *tree, PureMethod *m)
{
    std::string name;
    std::string type;
    bool wait_colon = true;
    iterateTree(tree, [&] (mpc_ast_t *item) {
        if (expect(item, "identifier")) {
            if (wait_colon) name += item->contents;
            else type += item->contents;
        }
        else if (wait_colon && expect(item, "char", ":"))
            wait_colon = false;
    });
    m_last_indent = name;
    return new TypeIdent(name, type);
}

void Compiler::parseStruct(mpc_ast_t *tree)
{
    Struct *s = nullptr;

    iterateTree(tree, [&] (mpc_ast_t *item) {
        if (!s && expect(item, "identifier"))
            s = new Struct(item->contents);
        else if (s && expect(item, "typeident"))
            s->addData(parseTypeIdent(item, nullptr));
        else printError("Unknown node in struct", item);
    });
    m_structs.push_back(s);
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

std::vector<Statement*> Compiler::codegen(mpc_ast_t *tree, PureMethod *m, int level)
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
        parseMethod(tree, level);
        recurse = false;
        level = 0;
    } else if (expect(tree, "struct")) {
        parseStruct(tree);
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
        rdata.push_back(parseMethodCall(tree));
        recurse = false;
    } else if (expect(tree, "assignment")) {
        AssignmentParser p(this, tree, m);
        rdata.push_back(p.parse());
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
        TypeIdent *ident = parseTypeIdent(tree, m);
        if (m_parameters) {
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
            NamespaceDef *def = parseNamespaceDef(tree);
            if (def != nullptr) {
                rdata.push_back(def);
            } else if (!cnts.empty()) {
                rdata.push_back(new Identifier(cnts));
            }
        }
        recurse = false;
    } else if (expect(tree, "identifier")) {
        // FIXME Some idenfiers are special/reserved words
        if (cnts == "false" || cnts == "true") {
            rdata.push_back(new Boolean(cnts));
        } else {
            rdata.push_back(new Identifier(cnts));
        }
    } else if (expect(tree, "import")) {
        addImport(tree);
        recurse = false;
    } else if (expect(tree, "const")) {
        addConst(tree);
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

void Compiler::parseParamDef(mpc_ast_t *tree, PureMethod *m, int level)
{
    iterateTree(tree, [&] (mpc_ast_t *item) {
        std::string cnts = item->contents;
        if (expect(item, "typeident")) {
            m_parameters = true;
            auto res = codegen(item, m, level + 1);
            m_parameters = false;
            for (auto r : res){
                if (r->type() == "TypeIdent")
                    m->addParameter(static_cast<TypeIdent*>(r));
                else
                    throw std::string("Invalid parameter definition: " + r->code());
            }
        } else if (expect(item, "char") && cnts == ",") {
            // FIXME Can ',' separate in params something else than next param?
        } else printError("Unknown node in parameter", item);
    });
}

void Compiler::parseArgs(mpc_ast_t *tree, PureMethod *m, int level)
{
    int open = 0;
    iterateTree(tree, [&] (mpc_ast_t *item) {
        if (expect(item, "char")) {
            std::string cnts = item->contents;
            if (cnts == "(") open++;
            else if (cnts == ")") open--;
            else {
                throw std::string("Unexpected char: " + cnts);
            }
        } else if (open > 0 && expect(item, "paramdef"))
            parseParamDef(item, m, level + 1);
        else printError("Unknown node in arguments", item);
    });
}

void Compiler::parseMethodRet(mpc_ast_t *tree, PureMethod *m, int level)
{
    auto r = codegen(tree, m, level);

    if (r.size() > 1) {
        throw std::string("Expected one return type, got " + std::to_string(r.size()) + " for '" + m->name() + "'");
    }

    if (r.size() == 1) {
        if (r[0]->type() != "Identifier") {
            throw std::string("Expected identifier as return type, got " + r[0]->type() + " for '" + m->name() + "'");
        }
        m->setReturnType(TypeDef(r[0]->code()));
    }
}

void Compiler::parseMethod(mpc_ast_t *tree, int level)
{
    PureMethod *m = new PureMethod();
    bool waitName = true;
    bool waitBody = false;
    iterateTree(tree, [&] (mpc_ast_t *item) {
        std::string tag = item->tag;
        std::string cnts = item->contents;
        if (waitName && expect(item, "pure")) {
            m->setPure();
        } else if (waitName && expect(item, "identifier")) {
            m->setName(cnts);
            waitName = false;
        } else if (!waitName && expect(item, "args")) {
            parseArgs(item, m, level + 1);
        } else if (!waitName && expect(item, "methodret")) {
            parseMethodRet(item, m, level + 1);
        } else if (expect(item, "ows")) {
            // Optional whitespace
        } else if (!waitBody && expect(item, "string", "=>")) {
            // Body should follow
            waitBody = true;
        } else if (waitBody && expect(item, "body")) {
            m->setBody(codegen(item, m, level));
            if (!m_blocks.empty()) {
                m->addBlock(m_blocks);
                m_blocks = std::vector<std::vector<Statement*>>();
            }
        } else if (expect(item, "newline") || expect(item, "ws")) {
        } else printError("Unknown node in method", item);
    });
    m_methods[m->name()] = m;
}
