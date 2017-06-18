#pragma once

#include <functional>
#include <string>
#include <vector>
#include <3pp/mpc/mpc.h>

namespace nolang
{

std::vector<std::string> applyToVector(std::vector<std::string> &dst, const std::vector<std::string> &src);

template<typename T> std::vector<T> applyToVector(std::vector<T> &dst, const std::vector<T> &src) {
    dst.insert(dst.end(), src.begin(), src.end());
    return dst;
}

void iterateTree(mpc_ast_t *tree, std::function<void(mpc_ast_t *)> closure);
bool expect(mpc_ast_t *tree, std::string key, std::string val="");
void printError(std::string, mpc_ast_t *);

#define iterate(A, B, C) iterateTree(B, [&] (mpc_ast_t *i) {\
    A = i;\
    C();\
});

}
