#include "moduledef.hh"
#include <iostream>

using nolang::ModuleDef;
using nolang::ModuleMethodDef;

ModuleDef::ModuleDef(std::string name) :
    m_name(name),
    m_parent(nullptr)
{
    m_ok = reload();
}

const std::string ModuleDef::mangleName(const std::string &name, const std::vector<std::string> &params)
{
    std::string res = name;

    for (const std::string &p : params) {
        res += "_" + p;
    }

    return res;
}

void ModuleDef::addMethod(ModuleMethodDef *m)
{
    m->setModule(this);
    m_methods.push_back(m);
}

const std::string ModuleDef::fullName() const
{
    std::string res;
    if (m_parent) res += m_parent->fullName();
    if (!res.empty()) res += '_';
    res += m_name;
    return res;
}

std::vector<std::string> ModuleDef::replaceParams(ModuleMethodDef *def, const std::vector<std::string> &params) const
{
    std::vector<std::string> res;
    for (auto p : params) {
        auto r = def->getCast(p);
        res.push_back(r);
    }
    return res;
}

ModuleMethodDef *ModuleDef::getMethod(std::string name, std::vector<std::string> params) const
{
    for (auto m : m_methods) {
        if (m->name() == name) {
            std::vector<std::string> mparams = replaceParams(m, params);
            std::string mangled = mangleName(name, mparams);
            std::cerr << "RR " << mangled << " == " << m->mangledName() << "\n";
            if (mangled == m->mangledName()) return m;
        }
    }
    return nullptr;
}

bool ModuleDef::reload()
{
    // FIXME proper module loader with JSON support
    if (m_name == "sys") return sysLoad();

    return false;
}

bool ModuleDef::sysLoad()
{
    // Sys should be only built-in module
    // Init code
    m_init_code = "/* sys init code */\n";
    m_init_code += "const char *sys_args_uint32(uint32_t index) {\n";
    m_init_code += "    if (index >= __argc) return NULL;\n";
    m_init_code += "    return __argv[index];\n";
    m_init_code += "}\n";

    ModuleMethodDef *args = new ModuleMethodDef("args");
    args->addParam("uint32");

    // Casting rules
    args->addCast("uint", "uint32");
    args->addCast("int", "uint");
    args->addCast("float", "uint");
    args->addCast("double", "uint");

    addMethod(args);

    return true;
}

const std::string ModuleMethodDef::getCastOnce(const std::string &param) const
{
    auto p = m_cast.find(param);
    if (p != m_cast.end()) { return p->second; }

    if (param == "uint8" ||
        param == "uint16" ||
        param == "uint32" ||
        param == "uint64") {
        auto p = m_cast.find("uint");
        if (p != m_cast.end()) { return getCast(p->second); }
    }
    if (param == "int8" ||
        param == "int16" ||
        param == "int32" ||
        param == "int64") {
        auto p = m_cast.find("int");
        if (p != m_cast.end()) { return p->second; }
    }
    if (param == "Number") {
        auto p = m_cast.find("int");
        if (p != m_cast.end()) { return p->second; }
        p = m_cast.find("uint");
        if (p != m_cast.end()) { return p->second; }
        // FIXME rest
    }

    return param;
}

const std::string ModuleMethodDef::getCast(const std::string &param) const
{
    std::string r = getCastOnce(param);
    int32_t cnt = m_cast.size();
    while (true) {
        std::string r2 = getCastOnce(r);
        --cnt;
        if (r == r2 || cnt <= 0) return r2;
    }
}

const std::string ModuleMethodDef::mangledName() const
{
    return ModuleDef::mangleName(m_name, m_params);
}

const std::string ModuleMethodDef::fullName() const
{
    std::string base;
    if (m_module) {
        base = m_module->fullName();
        if (!base.empty()) base += '_';
    }
    return ModuleDef::mangleName(base + m_name, m_params);
}

