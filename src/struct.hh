#pragma once

#include "statement.hh"

namespace nolang {

class Struct : public Statement
{
public:
    Struct(std::string val) : Statement("Struct", val) {}

    void addData(TypeIdent *def)
    {
        m_datas.push_back(def);
    }

    const std::vector<TypeIdent *> datas() const
    {
        return m_datas;
    }

protected:
    std::vector<TypeIdent *> m_datas;
};

}
