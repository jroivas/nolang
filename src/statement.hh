#pragma once

#include <string>

namespace nolang
{

class Statement
{
public:
    Statement() : m_type("Statement") {}
    Statement(std::string code)
     : m_type("Statement"),
       m_code(code)
    {
    }

    Statement(std::string type, std::string code)
     : m_type(type),
       m_code(code)
    {
    }

    std::string type() const { return m_type; }
    std::string code() const { return m_code; }

protected:
    std::string m_type;
    std::string m_code;
};

class StringValue : public Statement
{
public:
    StringValue(std::string val) : Statement("String", val) {}
};

class NumberValue : public Statement
{
public:
    NumberValue(std::string val) : Statement("Number", val) {}
};

class Op : public Statement
{
public:
    Op(std::string val) : Statement("Op", val) {}
};

}
