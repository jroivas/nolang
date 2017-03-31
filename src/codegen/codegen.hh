#pragma once

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

    virtual std::string generateConst(Statement *) = 0;
    virtual std::string generateImport(std::string) = 0;
    virtual std::string generateMethod(PureMethod *) = 0;
};

}
