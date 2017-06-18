#pragma once

#include "statement.hh"

namespace nolang {

class NamespaceDef : public Statement
{
public:
    NamespaceDef();
    NamespaceDef(std::string val);
    NamespaceDef(std::vector<std::string> val);

    void setValues(std::vector<std::string> val);

    const std::string name() const;
    const std::vector<std::string> values() const;
    void setCast(std::string to);
    const std::string cast() const;
    bool isValid() const
    {
        return !m_values.empty();
    }

protected:
    std::vector<std::string> m_values;
    std::string m_cast_to;
};

}
