#pragma once

#include <string>
#include <vector>

#include "statement.hh"

namespace nolang
{

class MethodCall : public Statement
{
public:
    MethodCall();

    void addNamespace(std::string ns)
    {
        m_namespace.push_back(ns);
    }

    void setNamespace(std::vector<std::string> ns)
    {
        m_namespace = ns;
    }

    void addParameter(std::vector<Statement*> p)
    {
        m_params.push_back(p);
    }

    std::vector<std::string> namespaces() const
    {
        return m_namespace;
    }

    std::vector<std::vector<Statement*>> params() const
    {
        return m_params;
    }

protected:
    std::vector<std::string> m_namespace;
    std::vector<std::vector<Statement*>> m_params;
};

}
