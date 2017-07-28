#pragma once

#include "statement.hh"
#include "puremethod.hh"
#include "cgen.hh"

namespace nolang {

class BlockGenerator
{
public:
    BlockGenerator(Cgen *c, const std::vector<std::vector<Statement *>> &b, const std::string &r, const PureMethod *m) :
        cgen(c),
        block(b),
        ret(r),
        method(m)
    {}
    std::vector<std::string> generate();

private:
    Cgen *cgen;
    const std::vector<std::vector<Statement *>> block;
    const std::string &ret;
    const PureMethod *method;

    void reset();
    bool isEOS(std::string) const;
    void handleEOS();
    void handleBlock(std::vector<std::string>);

    std::vector<std::string> lines;
    std::string resline;
};

}
