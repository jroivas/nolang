#pragma once

#include <map>
#include <string>
#include <vector>

namespace nolang
{
class ModuleMethodDef;

class ModuleDef
{
public:
    ModuleDef(std::string name);
    ~ModuleDef() {}

    bool reload();
    
    const std::string name() const
    {
        return m_name;
    }

    void addMethod(ModuleMethodDef *m)
    {
        m_methods.push_back(m);
    }

    const std::string initCode() const
    {
        return m_init_code;
    }
    bool ok() const
    {
        return m_ok;
    }
    static const std::string mangleName(const std::string &name, const std::vector<std::string> &params);

    ModuleMethodDef *getMethod(std::string name, std::vector<std::string> params);

private:
    std::vector<std::string> replaceParams(ModuleMethodDef *def, const std::vector<std::string> &params);
    bool sysLoad();
    bool m_ok;

    std::string m_name;

    std::string m_init_code;

    std::vector<std::string> m_libraries;
    std::vector<std::string> m_sources;
    std::vector<std::string> m_consts;
    std::vector<ModuleMethodDef*> m_methods;
};

class ModuleMethodDef
{
public:
    ModuleMethodDef(std::string name) : m_name(name) {}

    void addParam(std::string type)
    {
        m_params.push_back(type);
    }

    void addCast(std::string from, std::string to)
    {
        m_cast[from] = to;
    }

    const std::string name() const
    {
        return m_name;
    }

    const std::string mangledName() const;

    const std::vector<std::string> params() const
    {
        return m_params;
    }

    const std::string getCast(const std::string &) const;

private:
    std::string m_name;
    std::vector<std::string> m_params;
    std::map<std::string, std::string> m_cast;
};

}
