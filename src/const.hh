#pragma once

#include "statement.hh"
#include "assignment.hh"
#include "typeident.hh"

namespace nolang {

class Const : public Statement
{
public:
    Const(TypeIdent *i, Assignment *v) :
        Statement("Const", ""),
        m_ident(i),
        m_assignment(v)
    {}

    const TypeIdent *ident() const
    {
        return m_ident;
    }

    const Assignment *assignment() const
    {
        return m_assignment;
    }

protected:
    TypeIdent *m_ident;
    Assignment *m_assignment;
};

}
