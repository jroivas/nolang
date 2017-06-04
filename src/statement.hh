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

class NamespaceDef : public Statement
{
public:
    NamespaceDef(std::string val) :
        Statement("NamespaceDef", val)
    {
        m_values.push_back(val);
    }
    NamespaceDef(std::vector<std::string> val) :
        Statement("NamespaceDef", val.front()),
        m_values(val) {}

    const std::string name() const
    {
        std::string res;
        for (auto p : m_values) {
            if (!res.empty()) res += '.';
            res += p;
        }
        return res;
    }

    const std::vector<std::string> values() const {
        return m_values;
    }

    void setCast(std::string to)
    {
        m_cast_to = to;
    }

    const std::string cast() const
    {
        return m_cast_to;
    }

protected:
    std::vector<std::string> m_values;
    std::string m_cast_to;
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
    const std::string varType() const
    {
        return m_var_type;
    }

protected:
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
    Assignment(std::string var) : Statement("Assignment", var), m_def(nullptr) {}
    Assignment(NamespaceDef *var) : Statement("Assignment", var->name()), m_def(var)  {}

    void addStatements(std::vector<Statement*> stmt) {
        for (auto s : stmt) {
            m_statements.push_back(s);
        }
    }

    const std::vector<Statement*> statements() const
    {
        return m_statements;
    }

    const NamespaceDef *def() const
    {
        return m_def;
    }

protected:
    std::vector<Statement*> m_statements;
    NamespaceDef *m_def;
};


class Const : public Statement
{
public:
    Const(TypeIdent *i, Assignment *v) :
        Statement("Const", ""),
        m_ident(i),
        m_assignment(v)
    {}

    const TypeIdent *ident() const
    {
        return m_ident;
    }

    const Assignment *assignment() const
    {
        return m_assignment;
    }

protected:
    TypeIdent *m_ident;
    Assignment *m_assignment;
};

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

class Struct : public Statement
{
public:
    Struct(std::string val) : Statement("Struct", val) {}

    void addData(TypeIdent *def)
    {
        m_datas.push_back(def);
    }

    const std::vector<TypeIdent *> datas() const
    {
        return m_datas;
    }

protected:
    std::vector<TypeIdent *> m_datas;
};

}
