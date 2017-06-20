#pragma once

#include <string>
#include <vector>

#include "statement.hh"
#include "typeident.hh"

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

    void setReturnType(TypeDef ret)
    {
        m_return = ret;
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

    void addVariable(TypeIdent *id)
    {
        m_variables.push_back(id);
    }

    const std::vector<TypeIdent*> variables() const
    {
        return m_variables;
    }

    void addParameter(TypeIdent *i)
    {
        m_params.push_back(i);
    }

    void setParameters(std::vector<TypeIdent*> p);

    const std::vector<TypeIdent*> params() const
    {
        return m_params;
    }

    void setPure() {
        m_pure = true;
    }

protected:
    bool m_pure;
    std::string m_name;
    TypeDef m_return;
    //std::vector<std::string> body;
    //std::string m_body;
    std::vector<Statement*> m_body;
    std::vector<TypeIdent*> m_variables;

    std::vector<TypeIdent*> m_params;
    //std::vector<std::vector<std::string>> m_blocks;
    std::vector<std::vector<std::vector<Statement*>>> m_blocks;
};

}
