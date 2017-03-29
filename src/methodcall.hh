#pragma once

#include <string>
#include <vector>

namespace nolang
{

class MethodCall
{
public:
    MethodCall();

    void addNamespace(std::string ns)
    {
        m_namespace.push_back(ns);
    }
    void setNamespace(std::vector<std::string> ns)
    {
        m_namespace = ns;
    }

protected:
    std::vector<std::string> m_namespace;
    std::vector<std::string> m_params;
};

}
