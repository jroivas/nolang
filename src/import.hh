#pragma once

#include "statement.hh"

namespace nolang {

class Import : public Statement
{
public:
    Import(std::string val) : Statement("Import", val) {}

    void addSub(std::string as)
    {
        m_tree.push_back(as);
    }

    void addAs(std::string as)
    {
        m_as = as;
    }

    const std::string as() const
    {
        return m_as;
    }

    const std::string val() const
    {
        std::string res;
        res += code();
        for (auto c : m_tree) {
            res += '.';
            res += c;
        }
        return res;
    }

protected:
    std::vector<std::string> m_tree;

    std::string m_as;
};

}
