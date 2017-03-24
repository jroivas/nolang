#include <iostream>
#include <vector>
#include <map>

#include <3pp/mpc/mpc.h>
#include "parser.hh"
#include "puremethod.hh"

std::string lvl(int l)
{
    std::string res = "";
    for (int i = 0; i < l; ++i) {
        res += " ";
    }
    return res;
}

void walk(mpc_ast_t *res, int l=0)
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


std::string codegen(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::vector<std::string> &blocks, int level=0, PureMethod *m=nullptr);

void parseMethod(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::vector<std::string> &blocks, int level)
{
    PureMethod *m = new PureMethod();
    bool waitName = true;
    bool waitBody = false;
    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        if (waitName && tag.find("identifier") != std::string::npos) {
            m->name = cnts;
            waitName = false;
        } else if (tag.find("ows") != std::string::npos) {
            // Optional whitespace
        } else if (!waitBody && tag.find("string") != std::string::npos && cnts == "=>") {
            // Body should follow
            waitBody = true;
        } else if (waitBody && tag.find("body") != std::string::npos) {
            m->body = codegen(tree->children[c], methods, blocks, level, m);
        } else if (tag.find("newline") != std::string::npos) {
        //} else if (cnts.length() == 0) {
            // SKIP
        } else {
            std::cerr << "***ERROR: Unknown node: " << tag << ": '" << cnts << "'\n";
        }
    }
    methods[m->name] = m;
}

/*
void ensureBlockLevel(std::map<int, std::string> &blocks, int level)
{
    auto item = blocks.find(level);
    if (item == blocks.end()) {
        blocks[level] = "";
    }
}
*/

//std::string codegen(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::map<int, std::string> &blocks, int level)
std::string codegen(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::vector<std::string> &blocks, int level, PureMethod *m)
{
    std::string res = "";

    std::string tag = tree->tag;
    std::string cnts = tree->contents;
    bool recurse = true;

    if (tag == ">") {
        //std::cout << "ROOT\n";
    } else if (tag.find("methoddef") != std::string::npos) {
        // New method
        parseMethod(tree, methods, blocks, level);
        recurse = false;
        level = 0;
    } else if (tag.find("indent") != std::string::npos) {
        int new_level = cnts.length();
        if (new_level != level) {
            if (m) {
                m->blocks.push_back(blocks);
                blocks = std::vector<std::string>();
            }
        }
        // FIME Blocks does not work this way
        //ensureBlockLevel(blocks, level);
        //std::cout << "LVL " << level << " '" << cnts << "'\n";
    } else if ((tag.find("regex") != std::string::npos && cnts.length() == 0) ||
               (tag.find("stmt") != std::string::npos && cnts.length() == 0) ||
               (tag.find("body") != std::string::npos && cnts.length() == 0)) {
        // SKIP
    } else if (tag.find("number") != std::string::npos) {
        res += cnts;
    } else if (tag.find("termop") != std::string::npos) {
        res += cnts;
    } else if (tag.find("newline") != std::string::npos) {
        // Commit?
        //ensureBlockLevel(blocks, level);
        blocks.push_back(res);
        res = "";
        //blocks[level].push_back(res);
    } else if (tag.find("ows") != std::string::npos) {
    } else {
        std::cerr << "***ERROR: Unknown node: " << tag << ": '" << cnts << "'\n";
    }

    if (recurse) {
        for (int c = 0; c < tree->children_num; ++c) {
            res += codegen(tree->children[c], methods, blocks, level);
        }
    }

    return res;
}

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << "file\n";
        return 1;
    }

    Parser p;
    mpc_result_t *res = p.readParse(argv[1]);
    if (res == nullptr) {
        mpc_err_print(res->error);
        mpc_err_delete(res->error);
        exit(1);
    }

    std::map<std::string, PureMethod*> methods;
    std::vector<std::string> blocks;
    
    mpc_ast_print(static_cast<mpc_ast_t*>(res->output));
    //walk(static_cast<mpc_ast_t*>(r.output));
    std::cout << codegen(static_cast<mpc_ast_t*>(res->output), methods, blocks) << "\n";
    mpc_ast_delete(static_cast<mpc_ast_t*>(res->output));
    for (auto i : methods) {
        std::cout << i.first << ":\n";
        std::cout << i.second->body << "\n";
        for (auto v : i.second->blocks) {
            for (auto w : v) {
                std::cout << "  " << w << ":\n";
            }
        }
    }

    return 0;
}
