#pragma once

#include <string>
#include <vector>

#include "statement.hh"
#include "namespacedef.hh"

namespace nolang
{

class MethodCall : public Statement
{
public:
    MethodCall();

    void setNamespace(NamespaceDef *ns)
    {
        m_namespace = ns;
    }

    void addParameter(std::vector<Statement*> p)
    {
        m_params.push_back(p);
    }

    const NamespaceDef *namespaces() const
    {
        return m_namespace;
    }

    const std::vector<std::vector<Statement*>> params() const
    {
        return m_params;
    }

protected:
    NamespaceDef *m_namespace;
    std::vector<std::vector<Statement*>> m_params;
};

}
