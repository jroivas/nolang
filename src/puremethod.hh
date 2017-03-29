#pragma once

#include <string>
#include <vector>

#include "variable.hh"

namespace nolang
{

class PureMethod
{
public:
    PureMethod() : m_pure(false) {}
    bool m_pure;
    std::string m_name;
    //std::vector<std::string> body;
    std::string m_body;

    std::vector<Variable> m_params;
    std::vector<std::vector<std::string>> m_blocks;
};

}
