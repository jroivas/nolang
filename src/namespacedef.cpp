#include "namespacedef.hh"

using namespace nolang;

NamespaceDef::NamespaceDef() :
    Statement("NamespaceDe", "")
{
}

NamespaceDef::NamespaceDef(std::string val) :
    Statement("NamespaceDef", val)
{
    m_values.push_back(val);
}

NamespaceDef::NamespaceDef(std::vector<std::string> val) :
    Statement("NamespaceDef", val.front()),
    m_values(val)
{
}

void NamespaceDef::setValues(std::vector<std::string> val)
{
    m_code = val.front();
    m_values = val;
}

const std::string NamespaceDef::name() const
{
    std::string res;
    for (auto p : m_values) {
        if (!res.empty()) res += '.';
        res += p;
    }
    return res;
}

const std::vector<std::string> NamespaceDef::values() const
{
    return m_values;
}

void NamespaceDef::setCast(std::string to)
{
    m_cast_to = to;
}

const std::string NamespaceDef::cast() const
{
    return m_cast_to;
}
