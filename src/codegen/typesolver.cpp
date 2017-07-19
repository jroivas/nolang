#include "typesolver.hh"
#include "typeident.hh"
#include "nativetypesolver.hh"
#include "nolangtypesolver.hh"

#include <iostream>

using namespace nolang;

TypeSolver::TypeSolver() :
    method(nullptr)
{
}

TypeSolver::TypeSolver(const PureMethod *m) :
    method(m)
{
}

bool TypeSolver::isNative(const std::string & s)
{
    if (s.substr(0, 3) == "int" ||
        s.substr(0, 4) == "uint" ||
        s == "Double" ||
        s == "f64" ||
        s == "f32" ||
        s == "boolean" ||
        s == "string" ||
        s == "String") {
        return true;
    }
    return false;
}


std::string TypeSolver::native(const std::string & s)
{
    // Defaulting "int" to int32
    if (s == "int32" || s == "int") {
        return "int32_t";
    } else if (s == "int8") {
        return "int8_t";
    } else if (s == "int16") {
        return "int16_t";
    } else if (s == "int64") {
        return "int64_t";
    } else if (s == "uint32") {
        return "uint32_t";
    } else if (s == "uint8") {
        return "uint8_t";
    } else if (s == "uint16") {
        return "uint16_t";
    } else if (s == "uint64") {
        return "uint64_t";
    } else if (s == "Double") {
        return "double";
    } else if (s == "f64") {
        return "double";
    } else if (s == "f32") {
        return "float";
    } else if (s == "boolean") {
        return "boolean";
    } else if (s == "string" || s == "String") {
        return "const char *";
    } else {
        return s + " *";
    }
    throw "Unknown native type: " + s;
}

TypeIdent *TypeSolver::solveVariable(const std::string &name) const
{
    if (!method) return nullptr;
    for (auto var : method->variables()) {
        if (var->code() == name) {
            return var;
        }
    }
    return nullptr;
}

std::string TypeSolver::native(const Statement *s) const
{
    return NativeTypeSolver(this, s).solve();
}

std::string TypeSolver::nolangType(const Statement *s) const
{
    return NolangTypeSolver(this, s).solve();
}

std::string TypeSolver::typeOfChainImpl(std::vector<Statement*> chain, std::function<std::string(const Statement*)>& func) const
{
    std::string res;
    for (auto c : chain) {
        std::string t = func(c);
        if (!t.empty()) {
            if (res.empty()) res = t;
            else if (res == t) { /* OK */ }
            else {
                // Need to solve
                std::cerr << "*** ERROR: Can't solve type of chain, conflicting types: " << res << ", " << t << "\n";
            }
        }
    }
    return res;
}

std::string TypeSolver::typeOfChain(std::vector<Statement*> chain) const
{
    std::function<std::string(const Statement*)> f = [&](const Statement *s) { return this->native(s); };
    return typeOfChainImpl(chain, f);
}

std::string TypeSolver::nolangTypeOfChain(std::vector<Statement*> chain) const
{
    std::function<std::string(const Statement*)> f = [&](const Statement *s) { return this->nolangType(s); };
    return typeOfChainImpl(chain, f);
}
