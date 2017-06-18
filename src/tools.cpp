#include "tools.hh"
#include <iostream>

std::vector<std::string> nolang::applyToVector(std::vector<std::string> &dst, const std::vector<std::string> &src) {
    dst.insert(dst.end(), src.begin(), src.end());
    return dst;
}

void nolang::iterateTree(mpc_ast_t *tree, std::function<void(mpc_ast_t *)> closure) {
    for (int c = 0; c < tree->children_num; ++c)
        closure(tree->children[c]);
}

bool nolang::expect(mpc_ast_t *tree, std::string key, std::string val)
{
    std::string tag = tree->tag;

    if (tag.find(key) == std::string::npos) return false;
    if (!val.empty() && tree->contents != val) return false;

    return true;
}

void nolang::printError(std::string message, mpc_ast_t *item)
{
    std::cerr << "** ERROR: " << message << ": " << item->tag << ": '" << item->contents << "'\n";
}
