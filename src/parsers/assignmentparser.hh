#pragma once

#include <3pp/mpc/mpc.h>
#include "assignment.hh"
#include "compiler.hh"
#include "puremethod.hh"

namespace nolang {

class AssignmentParser
{
public:
    AssignmentParser(Compiler *, mpc_ast_t *, PureMethod *);
    Assignment *parse();

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
    mpc_ast_t *tree;
    mpc_ast_t *item;
    PureMethod *method;
    Assignment *assignment;

    bool wait_for_ident;
    bool wait_for_assign;
};

}
