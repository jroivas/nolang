#pragma once

#include <string>
#include <vector>

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

    Statement(const Statement &other)
     : m_type(other.m_type),
       m_code(other.m_code)
    {
    }

    bool primitive() const
    {
        if (m_type == "String") return true;
        if (m_type == "Number") return true;
        if (m_type == "Op") return true;
        return false;
    }

    const std::string type() const { return m_type; }
    const std::string code() const { return m_code; }

protected:
    std::string m_type;
    std::string m_code;
};

class Identifier : public Statement
{
public:
    Identifier(std::string val) : Statement("Identifier", val) {}
};

class Comparator : public Statement
{
public:
    Comparator(std::string val) : Statement("Comparator", val) {}
};

class Braces : public Statement
{
public:
    Braces(std::string val) : Statement("Braces", val) {}
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

class TypeDef : public Statement
{
public:
    TypeDef(std::string val) : Statement("TypeDef", val) {}
};

class VoidType : public TypeDef
{
public:
    VoidType() : TypeDef("void") {}
};

class EOS : public Statement
{
public:
    EOS() : Statement("EOS", "EOS") {}
};


class Boolean : public Statement
{
public:
    Boolean(std::string val) : Statement("Boolean", val) {}
};

}
