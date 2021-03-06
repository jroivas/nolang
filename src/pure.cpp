#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <utility>

#include <3pp/mpc/mpc.h>
#include "parser.hh"
#include "statement.hh"
#include "compiler.hh"
#include "puremethod.hh"
#include "codegen/cgen.hh"

mpc_result_t *parse(std::string file)
{
    nolang::Parser p;
    mpc_result_t *res = p.readParse(file.c_str());
    if (!p.success()) {
        mpc_err_print(res->error);
        mpc_err_delete(res->error);
        return nullptr;
    }
    return res;
}

bool doCompile(nolang::Compiler &c, mpc_result_t *res)
{
    try {
        c.codegen(static_cast<mpc_ast_t*>(res->output));
        mpc_ast_delete(static_cast<mpc_ast_t*>(res->output));
    }
    catch (char const *m) {
        std::cout << "== ERROR: Compile: " << m << "\n";
        return false;
    }
    catch (std::string m) {
        std::cout << "== ERROR: Compile: " << m << "\n";
        return false;
    }
    return true;
}

bool doCodeGen(nolang::Compiler &c)
{
    try {
        nolang::Cgen cgen;
        std::cout << cgen.generateUnit(&c) << "\n";
    }
    catch (char const *m) {
        std::cout << "== ERROR: Code gen: " << m << "\n";
        return false;
    }
    catch (std::string m) {
        std::cout << "== ERROR: Code gen: " << m << "\n";
        return false;
    }
    return true;
}

bool isDebug()
{
    if (const char *env_debug = std::getenv("DEBUG")) {
        std::string do_debug = env_debug;
        std::transform(do_debug.begin(), do_debug.end(), do_debug.begin(), ::toupper);
        if (do_debug == "TRUE" ||
            do_debug == "1") {
            return true;
        }
    }
    return false;
}

void printAst(mpc_result_t *res)
{
    std::cout << "/*\n";
    mpc_ast_print(static_cast<mpc_ast_t*>(res->output));
    std::cout << "*/\n";
}

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << "file\n";
        return 1;
    }

    mpc_result_t *res = parse(argv[1]);
    if (res == nullptr) return 1;

    if (isDebug()) printAst(res);

    nolang::Compiler c;

    if (!doCompile(c, res)) return 1;
    if (!doCodeGen(c)) return 1;

    //c.dump();

    return 0;
}
