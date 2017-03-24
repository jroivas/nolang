#include <iostream>
#include <vector>
#include <map>

#include <3pp/mpc/mpc.h>
#include "parser.hh"
#include "compiler.hh"
#include "puremethod.hh"

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << "file\n";
        return 1;
    }

    nolang::Parser p;
    mpc_result_t *res = p.readParse(argv[1]);
    if (res == nullptr) {
        mpc_err_print(res->error);
        mpc_err_delete(res->error);
        exit(1);
    }

    nolang::Compiler c;

    mpc_ast_print(static_cast<mpc_ast_t*>(res->output));
    std::cout << c.codegen(static_cast<mpc_ast_t*>(res->output)) << "\n";
    mpc_ast_delete(static_cast<mpc_ast_t*>(res->output));

    c.dump();

    return 0;
}
