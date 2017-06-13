#include "tools.hh"

std::vector<std::string> nolang::applyToVector(std::vector<std::string> &dst, const std::vector<std::string> &src) {
    dst.insert(dst.end(), src.begin(), src.end());
    return dst;
}
