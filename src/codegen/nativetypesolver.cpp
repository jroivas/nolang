#include "nativetypesolver.hh"

#include <cmath>
#include <limits>
#include <iostream>

#include "trim.hh"

using namespace nolang;

std::string NativeTypeSolver::solveTypeDef() const
{
    std::string t = statement->code();
    if (t == "void") return "void";
    return solver->native(t);
}

std::string NativeTypeSolver::solveTypeIdent() const
{
    const TypeIdent *i = static_cast<const TypeIdent *>(statement);
    std::string n = solver->native(i->varType());
    // FIXME
    std::cerr << "*** INVALID TYPEIDENT\n";
    return "";
}

std::string NativeTypeSolver::solveIdentifier() const
{
    TypeIdent *var = solver->solveVariable(statement->code());
    if (var) return solver->native(var->varType());
    return "invalid";
}

std::string NativeTypeSolver::solveString() const
{
    return "char *";
}

double NativeTypeSolver::convertToDouble(const std::string &num) const
{
    try {
        return std::stod(num, nullptr);
    }
    catch (std::out_of_range r) {
        throw "Invalid number: " + num;
    }
}

std::string NativeTypeSolver::solveNegativeNumber(double value) const
{
    if (fabs(value) >= std::numeric_limits<int32_t>::max()) return "int64_t";
    return "int32_t";
}

std::string NativeTypeSolver::solvePositiveNumber(double value) const
{
    if (value >= std::numeric_limits<int32_t>::max()) return "uint64_t";
    return "uint32_t";
}

std::string NativeTypeSolver::solveNumber() const
{
    std::string num = statement->code();
    /* TODO FIXME binary
    if (num.substr(0,2) == "0b") {
        // TODO Convert binary to hex
    }
    */
    double value = convertToDouble(trim(num));
    if (isNegativeNumber(num)) return solveNegativeNumber(value);
    return solvePositiveNumber(value);
}

std::string NativeTypeSolver::solveBoolean() const
{
    return "boolean";
}

std::string NativeTypeSolver::solve() const
{
    if (isTypeDef()) return solveTypeDef();
    else if (isTypeIdent()) return solveTypeIdent();
    else if (isIdentifier()) return solveIdentifier();
    else if (isString()) return solveString();
    else if (isNumber()) return solveNumber();
    else if (isBoolean()) return solveBoolean();
    else throw std::string("Unknown native type: " + statement->type());
}
