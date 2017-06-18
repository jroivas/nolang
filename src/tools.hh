#pragma once

#include <string>
#include <vector>

namespace nolang
{

std::vector<std::string> applyToVector(std::vector<std::string> &dst, const std::vector<std::string> &src);

template<typename T> std::vector<T> applyToVector(std::vector<T> &dst, const std::vector<T> &src) {
    dst.insert(dst.end(), src.begin(), src.end());
    return dst;
}
}
