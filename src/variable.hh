#pragma once

#include <string>

namespace nolang
{

class Variable
{
public:
    Variable(std::string name, std::string type="")
        : m_name(name),
          m_type(type)
    {
    }

    void setType(std::string type)
    {
        m_type = type;
    }

    std::string name() const { return m_name; }
    std::string type() const { return m_type; }

protected:
    std::string m_name;
    std::string m_type;
};
}
