#pragma once

#include "statement.hh"
#include "namespacedef.hh"

namespace nolang {

class Assignment : public Statement
{
public:
    Assignment(std::string var) : Statement("Assignment", var), m_def(nullptr) {}
    Assignment(NamespaceDef *var) : Statement("Assignment", var->name()), m_def(var)  {}

    void addStatements(std::vector<Statement*> stmt) {
        for (auto s : stmt) {
            m_statements.push_back(s);
        }
    }

    const std::vector<Statement*> statements() const
    {
        return m_statements;
    }

    const NamespaceDef *def() const
    {
        return m_def;
    }

protected:
    std::vector<Statement*> m_statements;
    NamespaceDef *m_def;
};

}
