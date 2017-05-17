#pragma once

#include <string>
#include "statement.hh"
#include "puremethod.hh"
#include "compiler.hh"

namespace nolang
{

/**
 * Abstract code generator.
 * Implement new code gen backend by implementing this interface.
 */
class CodeGen
{
public:
    CodeGen() {}
    virtual ~CodeGen() {}

    virtual std::string generateConst(const Statement *) = 0;
    virtual std::string generateImport(const std::string &) = 0;
    virtual std::string generateMethod(const PureMethod *) = 0;

    virtual std::string generateUnit(const Compiler *c) = 0;
};

}
