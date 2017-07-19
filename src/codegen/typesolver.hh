#pragma once

#include <string>
#include "puremethod.hh"
#include "statement.hh"

namespace nolang
{

class TypeSolver
{
public:
    TypeSolver();
    TypeSolver(const PureMethod *);
    static bool isNative(const std::string & s);
    static std::string native(const std::string & s);
    std::string native(const Statement *s) const;
    std::string nolangType(const Statement *s) const;
    std::string typeOfChain(std::vector<Statement*> chain) const;
    std::string nolangTypeOfChain(std::vector<Statement*> chain) const;
    TypeIdent *solveVariable(const std::string &name) const;

private:

    const PureMethod *method;
};

class NativeTypeSolver
{
public:
    NativeTypeSolver(const TypeSolver *t, const Statement *s) :
        solver(t),
        statement(s) {}
    std::string solve() const;

private:
    bool isTypeDef() const { return statement->type() == "TypeDef"; }
    bool isTypeIdent() const { return statement->type() == "TypeIdent"; }
    bool isIdentifier() const { return statement->type() == "Identifier"; }
    bool isString() const { return statement->type() == "String" || statement->type() == "string"; }
    bool isNumber() const { return statement->type() == "Number"; }
    bool isNegativeNumber(const std::string &n) const { return n[0] == '-'; }

    std::string solveTypeDef() const;
    std::string solveTypeIdent() const;
    std::string solveIdentifier() const;
    std::string solveString() const;
    std::string solveNumber() const;
    std::string solveNegativeNumber(double) const;
    std::string solvePositiveNumber(double) const;

    double convertToDouble(const std::string &) const;

    const TypeSolver *solver;
    const Statement *statement;
};

}

