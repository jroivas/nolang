#include "nolangtypesolver.hh"

#include "typeident.hh"

using namespace nolang;

std::string NolangTypeSolver::solveTypeDef() const
{
    if (statement->code() == "void") return "void";
    return statement->code();
}

std::string NolangTypeSolver::solveTypeIdent() const
{
    return static_cast<const TypeIdent *>(statement)->varType();
}

std::string NolangTypeSolver::solveIdentifier() const
{
    TypeIdent *var = solver->solveVariable(statement->code());
    if (var) return var->varType();
    return "invalid";
}

std::string NolangTypeSolver::solveString() const
{
    return "String";
}

std::string NolangTypeSolver::solveNumber() const
{
    return "int32";
}

std::string NolangTypeSolver::solveBoolean() const
{
    return "boolean";
}

std::string NolangTypeSolver::solve() const
{
    if (isTypeDef()) return solveTypeDef();
    else if (isTypeIdent()) return solveTypeIdent();
    else if (isIdentifier()) return solveIdentifier();
    else if (isString()) return solveString();
    else if (isNumber()) return solveNumber();
    else if (isBoolean()) return solveBoolean();
    else throw std::string("Unknown Nolang type: " + statement->type());
}
