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
        //copy(other);
    }

    //virtual ~Statement() {}

    bool primitive() const
    {
        if (m_type == "String") return true;
        if (m_type == "Number") return true;
        if (m_type == "Op") return true;
        return false;
    }

    std::string type() const { return m_type; }
    std::string code() const { return m_code; }

protected:
    //virtual void copy(const Statement &other) {}
    std::string m_type;
    std::string m_code;
};

class Identifier : public Statement
{
public:
    Identifier(std::string val) : Statement("Identifier", val) {}
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

class TypeIdent : public Statement
{
public:
    TypeIdent(std::string val, std::string type) : Statement("TypeIdent", val), m_var_type(type) {}
    std::string m_var_type;
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

class Assignment : public Statement
{
public:
    Assignment(std::string var) : Statement("Assignment", var) {}

    void addStatements(std::vector<Statement*> stmt) {
        for (auto s : stmt) {
            m_statements.push_back(s);
        }
    }

    std::vector<Statement*> statements() const {
        return m_statements;
    }

    std::vector<Statement*> m_statements;
};

}
