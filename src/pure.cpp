#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>

#include <3pp/mpc/mpc.h>
#include "parser.hh"
#include "statement.hh"
#include "compiler.hh"
#include "puremethod.hh"
#include "codegen/cgen.hh"

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << "file\n";
        return 1;
    }

    nolang::Parser p;
    mpc_result_t *res = p.readParse(argv[1]);
    if (!p.success()) {
        mpc_err_print(res->error);
        mpc_err_delete(res->error);
        return 1;
    }

    nolang::Compiler c;

    bool debug = false;
    if(const char *env_debug = std::getenv("DEBUG")) {
        std::string do_debug = env_debug;
        if (do_debug == "TRUE" ||
            do_debug == "1") {
            debug = true;
        }
    }
    if (debug) {
        std::cout << "/*\n";
        mpc_ast_print(static_cast<mpc_ast_t*>(res->output));
        std::cout << "*/\n";
    }

    try {
        c.codegen(static_cast<mpc_ast_t*>(res->output));
        mpc_ast_delete(static_cast<mpc_ast_t*>(res->output));
    }
    catch (char const *m) {
        std::cout << "== ERROR: Compile: " << m << "\n";
        return 1;
    }
    catch (std::string m) {
        std::cout << "== ERROR: Compile: " << m << "\n";
        return 1;
    }

    try {
        nolang::Cgen cgen;
        std::cout << cgen.generateUnit(&c) << "\n";
    }
    catch (char const *m) {
        std::cout << "== ERROR: Code gen: " << m << "\n";
        return 1;
    }
    catch (std::string m) {
        std::cout << "== ERROR: Code gen: " << m << "\n";
        return 1;
    }

    //c.dump();

    return 0;
}
