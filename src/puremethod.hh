#pragma once

#include <string>
#include <vector>

#include "statement.hh"
#include "variable.hh"

namespace nolang
{

class PureMethod : public Statement
{
public:
    PureMethod()
        : Statement("Method", ""),
          m_pure(false),
          m_return(VoidType())
    {
    }

    std::string name() const
    {
        return m_name;
    }

    void setName(std::string n)
    {
        m_name = n;
    }

    const TypeDef *returnType() const
    {
        return &m_return;
    }

    const std::vector<std::vector<std::vector<Statement*>>> blocks() const
    {
        return m_blocks;
    }

    void addBlock(std::vector<std::vector<Statement*>> block)
    {
        m_blocks.push_back(block);
    }

    const std::vector<Statement*> body() const
    {
        return m_body;
    }

    void setBody(std::vector<Statement*> b)
    {
        m_body = b;
    }

protected:
    bool m_pure;
    std::string m_name;
    TypeDef m_return;
    //std::vector<std::string> body;
    //std::string m_body;
    std::vector<Statement*> m_body;

    std::vector<Variable> m_params;
    //std::vector<std::vector<std::string>> m_blocks;
    std::vector<std::vector<std::vector<Statement*>>> m_blocks;
};

}
