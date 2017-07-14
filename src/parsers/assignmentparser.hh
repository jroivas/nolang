#pragma once

#include <3pp/mpc/mpc.h>
#include "baseparser.hh"
#include "assignment.hh"
#include "compiler.hh"
#include "puremethod.hh"

namespace nolang
{

class AssignmentParser : public BaseParser<Assignment>
{
public:
    AssignmentParser(Compiler *, mpc_ast_t *, PureMethod *);

private:
    void reset();
    void gotIdentifier();
    void gotAssignment();

    void parseTypeIdent();
    void parseNamespaceDef();
    void parseStatements();
    void parseItem();

    bool isTypeIdent() const;
    bool isNamespaceDef() const;
    bool isAssignment() const;
    bool isStatement() const;

    Compiler *compiler;
    PureMethod *method;

    bool wait_for_ident;
    bool wait_for_assign;
};

}
