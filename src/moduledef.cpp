#include "moduledef.hh"

using nolang::ModuleDef;
using nolang::ModuleMethodDef;

ModuleDef::ModuleDef(std::string name) :
    m_name(name)
{
    m_ok = reload();
}

ModuleMethodDef *ModuleDef::getMethod(std::string name, std::vector<std::string> params)
{
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

const std::string ModuleMethodDef::mangled_name() const
{
    std::string res = m_name;

    for (const std::string &p : m_params) {
        res += "_" + p;
    }

    return res;
}
