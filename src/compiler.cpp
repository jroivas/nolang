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

void Compiler::addImport(mpc_ast_t *tree)
{
    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        if (tag.find("namespacedef") != std::string::npos) {
            std::string cnts = tree->children[c]->contents;
            m_imports.push_back(cnts);
        }
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
            std::cerr << "***ERROR: Unknown node: " << tag << ": '" << cnts << "'\n";
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
        if (wait_ns && tag.find("namespacedef") != std::string::npos) {
            mcall->setNamespace(parseNamespaceDef(tree->children[c]));
            wait_ns = false;
        }
        else if (!wait_call_end && tag.find("char") != std::string::npos && cnts == "(") {
            wait_call_end = true;
        }
        else if (wait_call_end && tag.find("char") != std::string::npos && cnts == ")") {
            wait_call_end = false;
        }
        else if (wait_call_end) {
            mcall->addParameter(codegen(tree->children[c]));
        }
        else {
            std::cerr << "***ERROR: Unknown node: " << tag << ": '" << cnts << "'\n";
        }
    }

    // TODO Return methodcall object
    return mcall;
}

std::vector<Statement*> Compiler::codegen(mpc_ast_t *tree, PureMethod *m, int level)
{
    std::vector<Statement*> rdata;

    std::string tag = tree->tag;
    std::string cnts = tree->contents;
    bool recurse = true;

    if (tag == ">") {
        //std::cout << "ROOT\n";
    } else if (tag.find("methoddef") != std::string::npos) {
        // New method
        parseMethod(tree, level);
        recurse = false;
        level = 0;
    } else if (tag.find("indent") != std::string::npos) {
        int new_level = cnts.length();
        if (new_level != level) {
            if (m) {
                m->m_blocks.push_back(m_blocks);
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
    } else if (tag.find("number") != std::string::npos) {
        rdata.push_back(new NumberValue(cnts));
    } else if (tag.find("string") != std::string::npos) {
        rdata.push_back(new StringValue(cnts));
    } else if (tag.find("identifier") != std::string::npos) {
        // FIXME Some idenfiers are special/reserved words
        rdata.push_back(new Identifier(cnts));
    } else if (tag.find("termop") != std::string::npos) {
        rdata.push_back(new Op(cnts));
    } else if (tag.find("import") != std::string::npos) {
        addImport(tree);
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
        std::cerr << "***ERROR: Unknown node: " << tag << ": '" << cnts << "'\n";
    }

    if (recurse) {
        for (int c = 0; c < tree->children_num; ++c) {
            std::vector<Statement*>  st = codegen(tree->children[c], m, level);
            //if (!st.empty()) {
            for (auto s : st) {
                if (s->type() == "EOS") {
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

void Compiler::parseMethod(mpc_ast_t *tree, int level)
{
    PureMethod *m = new PureMethod();
    bool waitName = true;
    bool waitBody = false;
    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        // TODO parameters
        if (waitName && tag.find("identifier") != std::string::npos) {
            m->m_name = cnts;
            waitName = false;
        } else if (tag.find("ows") != std::string::npos) {
            // Optional whitespace
        } else if (!waitBody && tag.find("string") != std::string::npos && cnts == "=>") {
            // Body should follow
            waitBody = true;
        } else if (waitBody && tag.find("body") != std::string::npos) {
            m->m_body = codegen(tree->children[c], m, level);
            if (!m_blocks.empty()) {
                m->m_blocks.push_back(m_blocks);
                //m_blocks = std::vector<std::string>();
                m_blocks = std::vector<std::vector<Statement*>>();
            }
        } else if (tag.find("newline") != std::string::npos) {
        //} else if (cnts.length() == 0) {
            // SKIP
        } else {
            std::cerr << "***ERROR: Unknown node: " << tag << ": '" << cnts << "'\n";
        }
    }
    m_methods[m->m_name] = m;
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
        std::cout << "  " << i << "\n";
    }

    std::cout << "== Methods\n";
    for (auto i : m_methods) {
        std::cout << i.first << ":\n";
        for (auto b : i.second->m_body) {
            dumpStatement(b, 2);
        }
        for (auto v : i.second->m_blocks) {
            std::cout << " VV \n";
            for (auto w : v) {
                std::cout << "  WW\n";
                for (auto z : w) {
                    std::cout << "   ZZ\n";
                    dumpStatement(z, 4);
                    //std::cout << "  " << z->type() << ": " << z->code() << ":\n";
                }
            }
        }
    }
}
