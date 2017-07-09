#include "typesolver.hh"
#include "typeident.hh"

#include <cmath>
#include <limits>
#include <iostream>

#include "trim.hh"

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

std::string TypeSolver::nativeFromStatement(const Statement *s) const
{
    if (s->type() == "TypeDef") {
        if (s->code() == "void") {
            return "void";
        }
        return native(s->code());
    } else if (s->type() == "TypeIdent") {
        const TypeIdent *i = static_cast<const TypeIdent *>(s);
        std::string n = native(i->varType());
        // FIXME
    } else if (s->type() == "Identifier") {
        TypeIdent *var = solveVariable(s->code());
        if (var) {
            return native(var->varType());
        }
        return "invalid";
    } else if (s->type() == "String" || s->type() == "string") {
        // FIXME "const char*" or "char*"
        return "char *";
    } else if (s->type() == "Number") {
        // FIXME type and size, floats
        std::string num = s->code();
        num = trim(num);
        /* TODO FIXME
        if (num.substr(0,2) == "0b") {
            // TODO Convert binary to hex
        }
        */
        double value = std::stod(num, nullptr);
        if (num[0] == '-') {
            if (fabs(value) >= std::numeric_limits<int32_t>::max()) {
                return "int64_t";
            }
            return "int32_t";
        } else {
            if (value >= std::numeric_limits<int32_t>::max()) {
                return "uint64_t";
            }
            return "uint32_t";
        }
    } else if (s->type() == "Boolean") {
        return "boolean";
    } else {
        throw std::string("Unknown native type: " + s->type());
    }
    return "";
}

std::string TypeSolver::nolangType(const Statement *s) const
{
    if (s->type() == "TypeDef") {
        if (s->code() == "void") {
            return "void";
        }
        return s->code();
        // FIXME
    } else if (s->type() == "TypeIdent") {
        const TypeIdent *i = static_cast<const TypeIdent *>(s);
        return i->varType();
    } else if (s->type() == "Identifier") {
        TypeIdent *var = solveVariable(s->code());
        if (var) {
            var->varType();
        }
        return "invalid";
    } else if (s->type() == "String") {
        return "String";
    } else if (s->type() == "Number") {
        // FIXME type and size, floats
        return "int32";
    } else if (s->type() == "Boolean") {
        return "boolean";
    } else {
        throw std::string("Unknown type: " + s->type());
    }
    return "";
}

// FIXME Combine with below
std::string TypeSolver::typeOfChain(std::vector<Statement*> chain) const
{
    std::string res;
    for (auto c : chain) {
        std::string t = nativeFromStatement(c);
        if (!t.empty()) {
            if (res.empty()) {
                res = t;
            } else if (res == t) {
                // OK
            } else {
                // Need to solve
                std::cerr << "*** ERROR: Can't solve type of chain, conflicting types: " << res << ", " << t << "\n";
            }
        }
    }
    return res;
}

std::string TypeSolver::nolangTypeOfChain(std::vector<Statement*> chain) const
{
    std::string res;
    for (auto c : chain) {
        std::string t = nativeFromStatement(c);
        if (!t.empty()) {
            if (res.empty()) {
                res = t;
            } else if (res == t) {
                // OK
            } else {
                // Need to solve
                std::cerr << "*** ERROR: Can't solve type of chain, conflicting types: " << res << ", " << t << "\n";
            }
        }
    }
    return res;
}

