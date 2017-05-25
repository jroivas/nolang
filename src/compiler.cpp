#include "compiler.hh"

#include <iostream>

using namespace nolang;

static std::string lvl(int l)
{
    std::string res = "";
    for (int i = 0; i < l; ++i) {
        res += " ";
    }
    return res;
}

static void walk(mpc_ast_t *res, int l=0)
{
    std::string tmp = res->contents;
    if (tmp.length() > 0) {
        tmp = ": '" + tmp + '\'';
    }
    std::cout
        << "0x" << std::hex << long(res) << lvl(l)
        << " " << res->tag
        <<  tmp << "\n";
    for (int c = 0; c < res->children_num; ++c) {
        walk(res->children[c], l + 1);
    }
}

Compiler::Compiler() :
    m_parameters(false)
{
}

Import *Compiler::addImportAs(mpc_ast_t *tree)
{
    Import *imp = nullptr;

    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        if (tag.find("identifier") != std::string::npos) {
            if (imp == nullptr) {
                imp = new Import(cnts);
            } else {
                imp->addSub(cnts);
            }
        }
    }

    return imp;
}

void Compiler::addImport(mpc_ast_t *tree)
{
    std::cout << "/*\n";
    mpc_ast_print(tree);
    std::cout << "*/\n";
    Import *imp = nullptr;
    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        if (tag.find("identifier") != std::string::npos) {
            if (imp != nullptr) {
                imp->addAs(cnts);
            } else {
                imp = new Import(cnts);
            }
        } else if (tag.find("namespacedef") != std::string::npos) {
            imp = addImportAs(tree->children[c]);
        }
    }
    if (imp) {
        m_imports.push_back(imp);
    }
}

void Compiler::addConst(mpc_ast_t *tree)
{
    for (int c = 0; c < tree->children_num; ++c) {
        /*
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        std::cout << " CCC " << tag << " = " << cnts << "\n";
        std::string tag = tree->children[c]->tag;
        if (tag.find("namespacedef") != std::string::npos) {
            std::string cnts = tree->children[c]->contents;
            m_imports.push_back(cnts);
        }
        */
    }
}

std::vector<std::string> Compiler::parseNamespaceDef(mpc_ast_t *tree)
{
    std::vector<std::string> res;
    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        if (tag.find("identifier") != std::string::npos) {
            res.push_back(cnts);
        }
        else {
            std::cerr << "** ERROR: Unknown node in namespace defination: " << tag << ": '" << cnts << "'\n";
        }
    }
    return res;
}

MethodCall *Compiler::parseMethodCall(mpc_ast_t *tree)
{
    MethodCall *mcall = new MethodCall();

    bool wait_ns = true;
    bool wait_call_end = false;

    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        if (wait_ns && tag.find("identifier") != std::string::npos) {
            std::vector<std::string> res;
            res.push_back(cnts);
            mcall->setNamespace(res);
        } else if (wait_ns && tag.find("namespacedef") != std::string::npos) {
            mcall->setNamespace(parseNamespaceDef(tree->children[c]));
            wait_ns = false;
        } else if (!wait_call_end && tag.find("char") != std::string::npos && cnts == "(") {
            wait_call_end = true;
        } else if (wait_call_end && tag.find("char") != std::string::npos && cnts == ")") {
            wait_call_end = false;
        } else if (wait_call_end) {
            mcall->addParameter(codegen(tree->children[c]));
        } else {
            std::cerr << "** ERROR: Unknown node in method call: " << tag << ": '" << cnts << "'\n";
        }
    }

    // TODO Return methodcall object
    return mcall;
}

bool Compiler::expect(mpc_ast_t *tree, std::string key, std::string val) const
{
    std::string tag = tree->tag;
    std::string cnts = tree->contents;

    if (tag.find(key) == std::string::npos) return false;
    if (!val.empty() && cnts != val) return false;

    return true;
}

TypeIdent *Compiler::parseTypeIdent(mpc_ast_t *tree, PureMethod *m, int level)
{
    std::string name;
    std::string type;
    bool wait_colon = true;
    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        if (expect(tree->children[c], "identifier")) {
            if (wait_colon) name += cnts;
            else type += cnts;
        }
        else if (wait_colon && expect(tree->children[c],"char", ":")) {
            wait_colon = false;
        }
    }
    m_last_indent = name;
    return new TypeIdent(name, type);
}

Assignment *Compiler::parseAssignment(mpc_ast_t *tree, PureMethod *m, int level)
{
    bool wait_for_ident = true;
    bool wait_for_assign = false;
    Assignment *ass = nullptr;
    for (int c = 0; c < tree->children_num; ++c) {
        if (wait_for_ident && expect(tree->children[c], "typeident")) {
            m->addVariable(parseTypeIdent(tree->children[c], m, level + 1));
            wait_for_ident = false;
            wait_for_assign = true;
            ass = new Assignment(m_last_indent);
        } else if (wait_for_ident && expect(tree->children[c], "namespacedef")) {
            m_last_indent = tree->children[c]->contents;
            wait_for_ident = false;
            wait_for_assign = true;
            ass = new Assignment(m_last_indent);
        } else if (wait_for_assign && expect(tree->children[c], "char", "=")) {
            wait_for_assign = false;
        } else if (!wait_for_ident && !wait_for_assign) {
            // Now need to parse statements/expr...
            std::vector<Statement*> stmt = codegen(tree->children[c], m, level + 1);
            ass->addStatements(stmt);
        } else {
            std::string tag = tree->children[c]->tag;
            std::string cnts = tree->children[c]->contents;
            std::cerr << "** ERROR: Unknown node in assignment: " << tag << ": '" << cnts << "'\n";
        }
    }

    return ass;
}

std::vector<Statement*> Compiler::codegen(mpc_ast_t *tree, PureMethod *m, int level)
{
    std::vector<Statement*> rdata;

    std::string tag = tree->tag;
    std::string cnts = tree->contents;
    bool recurse = true;

    if (tag == ">") {
        //std::cout << "ROOT\n";
    } else if (tag.find("comment") != std::string::npos) {
        // Ignore comments
        recurse = false;
    } else if (tag.find("methoddef") != std::string::npos) {
        // New method
        parseMethod(tree, level);
        recurse = false;
        level = 0;
    } else if (tag.find("indent") != std::string::npos) {
        int new_level = cnts.length();
        if (new_level != level) {
            if (m && !m_blocks.empty()) {
                m->addBlock(m_blocks);
                //m_blocks = std::vector<std::string>();
                m_blocks = std::vector<std::vector<Statement*>>();
            }
        }
        // FIME Blocks does not work this way
        //ensureBlockLevel(blocks, level);
        //std::cout << "LVL " << level << " '" << cnts << "'\n";
    /*} else if ((tag.find("regex") != std::string::npos && cnts.length() == 0) ||
               (tag.find("body") != std::string::npos && cnts.length() == 0)) {
        */
        // SKIP and recurse
    } else if (tag.find("methodcall") != std::string::npos) {
        rdata.push_back(parseMethodCall(tree));
        recurse = false;
    } else if (tag.find("assignment") != std::string::npos) {
        rdata.push_back(parseAssignment(tree, m, level + 1));
        recurse = false;
    } else if (tag.find("number") != std::string::npos) {
        rdata.push_back(new NumberValue(cnts));
    } else if (tag.find("termop") != std::string::npos) {
        rdata.push_back(new Op(cnts));
    } else if (tag.find("factorop") != std::string::npos) {
        rdata.push_back(new Op(cnts));
    } else if (tag.find("string") != std::string::npos) {
        rdata.push_back(new StringValue(cnts));
    } else if (tag.find("typeident") != std::string::npos) {
        TypeIdent *ident = parseTypeIdent(tree, m, level + 1);
        if (m_parameters) {
            rdata.push_back(ident);
        } else if (m != nullptr) {
            m->addVariable(ident);
        } else {
            rdata.push_back(ident);
        }
        recurse = false;
    } else if (tag.find("namespacedef") != std::string::npos) {
        rdata.push_back(new NamespaceDef(parseNamespaceDef(tree)));
        recurse = false;
    } else if (tag.find("identifier") != std::string::npos) {
        // FIXME Some idenfiers are special/reserved words
        std::cerr << " ID " << cnts << "\n";
        rdata.push_back(new Identifier(cnts));
    } else if (tag.find("import") != std::string::npos) {
        addImport(tree);
        recurse = false;
    } else if (tag.find("const") != std::string::npos) {
        addConst(tree);
        recurse = false;
    } else if (tag.find("newline") != std::string::npos) {
        // Commit?
        //ensureBlockLevel(blocks, level);
        //m_blocks.push_back(res);
        /*
        m_blocks.push_back(rdata);
        std::cout << "NL\n";
        for (auto b : rdata) {
            dumpStatement(b);
        }
        std::cout << "CC\n";
        rdata = std::vector<Statement*>();
        */
        rdata.push_back(new EOS());
    } else if (tag.find("ows") != std::string::npos ||
               tag.find("ws") != std::string::npos) {
    } else {
        std::cerr << "** ERROR: Unknown node in statement: " << tag << ": '" << cnts << "'\n";
    }

    if (recurse) {
        for (int c = 0; c < tree->children_num; ++c) {
            std::vector<Statement*>  st = codegen(tree->children[c], m, level);
            //if (!st.empty()) {
            for (auto s : st) {
                if (s->type() == "EOS") {
                    rdata.push_back(s);
                    m_blocks.push_back(rdata);
                    rdata = std::vector<Statement*>();
                } else {
                    rdata.push_back(s);
                }
            }
        }
    }

    return rdata;
}

void Compiler::parseParamDef(mpc_ast_t *tree, PureMethod *m, int level)
{
    int numparams = 0;
    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        if (expect(tree->children[c], "typeident")) {
            m_parameters = true;
            auto res = codegen(tree->children[c], m, level + 1);
            m_parameters = false;
            for (auto r: res ){
                if (r->type() == "TypeIdent") {
                    m->addParameter(static_cast<TypeIdent*>(r));
                } else {
                    throw std::string("Invalid parameter definition: " + r->code());
                }
            }
        } else if (expect(tree->children[c], "char") && cnts == ",") {
            // FIXME?
            numparams += 1;
        } else {
            std::cerr << "** ERROR: Unknown node in parameter: " << tag << ": '" << cnts << "'\n";
        }
    }
}

void Compiler::parseArgs(mpc_ast_t *tree, PureMethod *m, int level)
{
    int open = 0;
    for (int c = 0; c < tree->children_num; ++c) {
        std::string cnts = tree->children[c]->contents;
        if (expect(tree->children[c], "char")) {
            if (cnts == "(") open++;
            else if (cnts == ")") open--;
            else {
                throw std::string("Unexpected char: " + cnts);
            }
        } else if (open > 0 && expect(tree->children[c], "paramdef")) {
            parseParamDef(tree->children[c], m, level + 1);
        } else {
            std::string tag = tree->children[c]->tag;
            std::cerr << "** ERROR: Unknown node in arguments: " << tag << ": '" << cnts << "'\n";
        }
    }
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
    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        if (waitName && tag.find("pure") != std::string::npos) {
            m->setPure();
        } else if (waitName && tag.find("identifier") != std::string::npos) {
            m->setName(cnts);
            waitName = false;
        } else if (!waitName && expect(tree->children[c], "args")) {
            parseArgs(tree->children[c], m, level + 1);
        } else if (!waitName && expect(tree->children[c], "methodret")) {
            parseMethodRet(tree->children[c], m, level + 1);
        } else if (tag.find("ows") != std::string::npos) {
            // Optional whitespace
        } else if (!waitBody && tag.find("string") != std::string::npos && cnts == "=>") {
            // Body should follow
            waitBody = true;
        } else if (waitBody && tag.find("body") != std::string::npos) {
            m->setBody(codegen(tree->children[c], m, level));
            if (!m_blocks.empty()) {
                m->addBlock(m_blocks);
                //m_blocks = std::vector<std::string>();
                m_blocks = std::vector<std::vector<Statement*>>();
            }
        } else if (tag.find("newline") != std::string::npos || tag.find("ws") != std::string::npos) {
        //} else if (cnts.length() == 0) {
            // SKIP
        } else {
            std::cerr << "** ERROR: Unknown node in method: " << tag << ": '" << cnts << "'\n";
        }
    }
    m_methods[m->name()] = m;
}

void Compiler::dumpStatement(Statement *s, int l) const
{
    std::cout << lvl(l) << s->type() << ": ";
    if (s->type() == "MethodCall") {
        MethodCall *mc = static_cast<MethodCall*>(s);
        for (auto d : mc->namespaces()) {
            std::cout << d << " ";
        }
        std::cout << "\n";
        std::cout << lvl(l+1) << "Params:\n";
        for (auto d : mc->params()) {
            for (auto e : d) {
                dumpStatement(e, l + 2);
            }
        }
    } else if (s->type() == "String") {
        std::cout << s->code() << "\n";
    } else if (s->type() == "Number") {
        std::cout << s->code() << "\n";
    } else if (s->type() == "Op") {
        std::cout << s->code() << "\n";
    } else {
        std::cout << "Unknown statement type: " << s->type() << "\n";
    }
}

void Compiler::dump() const
{
    std::cout << "== Imports\n";
    for (auto i : m_imports) {
        std::cout << "  " << i->code() << "\n";
    }

    std::cout << "== Methods\n";
    for (auto i : m_methods) {
        std::cout << i.first << ":\n";
        for (auto b : i.second->body()) {
            dumpStatement(b, 2);
        }
        for (auto v : i.second->blocks()) {
            std::cout << " BLOCK \n";
            for (auto w : v) {
                //std::cout << "  WW\n";
                for (auto z : w) {
                    //std::cout << "   ZZ\n";
                    dumpStatement(z, 4);
                    //std::cout << "  " << z->type() << ": " << z->code() << ":\n";
                }
            }
        }
    }
}
