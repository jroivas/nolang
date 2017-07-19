#pragma once

#include <string>
#include <functional>
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
    std::string typeOfChainImpl(std::vector<Statement*> chain, std::function<std::string(const Statement*)>&) const;

    const PureMethod *method;
};

}
