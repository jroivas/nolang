#pragma once

#include <string>
#include <vector>
#include "statement.hh"

namespace nolang {

class TypeIdent : public Statement
{
public:
    TypeIdent(std::string val, std::string type) : Statement("TypeIdent", val), m_var_type(type) {}
    const std::string varType() const
    {
        return m_var_type;
    }

protected:
    std::string m_var_type;
};

}
