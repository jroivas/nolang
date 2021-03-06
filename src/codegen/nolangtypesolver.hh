#pragma once

#include <string>
#include "statement.hh"
#include "typesolver.hh"

namespace nolang
{

class NolangTypeSolver
{
public:
    NolangTypeSolver(const TypeSolver *t, const Statement *s) :
        solver(t),
        statement(s) {}

    std::string solve() const;

private:
    bool isTypeDef() const { return statement->type() == "TypeDef"; }
    bool isTypeIdent() const { return statement->type() == "TypeIdent"; }
    bool isIdentifier() const { return statement->type() == "Identifier"; }
    bool isString() const { return statement->type() == "String"; }
    bool isNumber() const { return statement->type() == "Number"; }
    bool isNegativeNumber(const std::string &n) const { return n[0] == '-'; }
    bool isBoolean() const { return statement->type() == "Boolean"; }

    std::string solveTypeDef() const;
    std::string solveTypeIdent() const;
    std::string solveIdentifier() const;
    std::string solveString() const;
    std::string solveNumber() const;
    std::string solveBoolean() const;

    const TypeSolver *solver;
    const Statement *statement;
};

}
