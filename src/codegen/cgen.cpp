#include "cgen.hh"
#include "tools.hh"

#include <iostream>
#include <sstream>
#include <vector>

#include "typesolver.hh"
#include "trim.hh"
#include "blockgenerator.hh"
#include "statementgenerator.hh"

using namespace nolang;
static const std::string __autogen_prefix = "__autogen_";
static unsigned int __autogen_index = 0;

Cgen::Cgen()
    : m_current_module(nullptr)
{
}

std::string Cgen::autogen()
{
    std::stringstream ss;
    ss << __autogen_prefix << ++__autogen_index;
    return ss.str();
}

const ModuleDef *Cgen::getModule(std::string name) const
{
    auto mod = m_modules.find(name);
    if (mod == m_modules.end()) return nullptr;

    return mod->second;
}

std::string Cgen::generateIOImport() const
{
    return "#include <stdio.h>\n";
}

std::string Cgen::generateModuleInitCode(std::string val, const ModuleDef *def)
{
    m_modules[val] = def;
    return def->initCode();
}

std::string Cgen::generateModuleFixme(const Import *import, std::string val, ModuleDef *def) const
{
    delete def;
    std::string res;
    res += "// FIXME: #include <" + val + ">";
    if (!import->as().empty()) {
        res += " as " + import->as();
    }
    res += "\n";
    std::cerr << "** ERROR: Unhandled import " << val << "\n";
    return res;
}

std::string Cgen::generateModuleImport(const Import *imp, std::string val)
{
    ModuleDef *def = new ModuleDef(val);
    if (isValidModuleDef(def)) return generateModuleInitCode(val, def);
    return generateModuleFixme(imp, val, def);
}

std::string Cgen::generateImport(const Import *imp)
{
    // FIXME Built-in import
    std::string val = imp->val();
    if (isBuiltInImport(val)) return generateIOImport();
    else if (!isValidImport(getModule(val))) return ""; //FIXME
    else return generateModuleImport(imp, val);
}

const Struct *Cgen::getStruct(const std::string &name) const
{
    for (auto s : m_structs) {
        if (s->code() == name) return s;
    }
    return nullptr;
}

bool Cgen::isStruct(const std::string &name) const
{
    for (auto s : m_structs) {
        if (s->code() == name) return true;
    }

    return false;
}

std::string Cgen::castCode(const std::string &src_var, const std::string &src_type, const std::string &to_type) const
{
    std::string res;
    if (src_type == "String") {
        if (to_type == "int" || to_type == "int32" || to_type == "int16" || to_type == "int8") {
            res += src_var + " == NULL ? 0 : strtol(" + src_var + ", NULL, 10)";
            return res;
        } else if (to_type == "int64") {
            res += src_var + " == NULL ? 0 : strtoll(" + src_var + ", NULL, 10)";
            return res;
        } else if (to_type == "uint" || to_type == "uint32" || to_type == "uint16" || to_type == "uint8") {
            res += src_var + " == NULL ? 0 : strtoul(" + src_var + ", NULL, 10)";
            return res;
        } else if (to_type == "int64") {
            res += src_var + " == NULL ? 0 : strtoull(" + src_var + ", NULL, 10)";
            return res;
        }
    }
    std::cerr << "** ERROR: Unhandled CAST " << src_var << " " << src_type << " -> " << to_type << "\n";
    // FIXME
    return res;
}

std::vector<std::string> Cgen::applyPostponed(std::vector<std::string> &res)
{
    if (!m_postponed_assignment.empty()) {
        res.push_back(m_postponed_assignment + " = ");
        m_postponed_assignment = "";
    }
    return res;
}

void Cgen::setPostponed(std::string s)
{
    m_postponed_assignment = s;
}

void Cgen::setPostponedMethod(std::string s)
{
    m_postponed_method = s;
}

void Cgen::appendPostponed(std::string s)
{
    m_postponed_assignment += s;
}

const std::string Cgen::postponed() const
{
    return m_postponed_assignment;
}

std::string Cgen::usePostponedMethod()
{
    std::string tmp = m_postponed_method;
    m_postponed_method = "";
    return tmp;
}

std::vector<std::string> Cgen::generateBlock(const std::vector<std::vector<Statement *>> &block, const std::string &ret, const PureMethod *m)
{
    return BlockGenerator(this, block, ret, m).generate();
}

std::vector<std::string> Cgen::generateVariable(const TypeIdent *i) const
{
    std::vector<std::string> res;

    std::string varType = i->varType();
    std::string native = TypeSolver::native(varType);

    res.push_back(native + " " + i->code() + ";\n");

    return res;
}

std::string Cgen::generateVariableInit(const TypeIdent *i)
{
    std::string varType = i->varType();
    if (!TypeSolver::isNative(varType)) {
        return i->code() + " = new_" + varType + "();\n";
    }
    return "";
}

std::string Cgen::solveReturnType(const Statement *t, const PureMethod *m) const
{
    std::string ret = TypeSolver(m).native(m->returnType());
    // FIXME Return type if not defined?
#if 1
    // FIXME Forcing main to be "int"
    if (m->name() == "main") {
        //
        if (ret == "void") ret = "int";
    }
#endif
    return ret;
}

std::string Cgen::generateMethodPrototype(const PureMethod *m)
{
    std::string ret = solveReturnType(m->returnType(), m);

    std::string param_str;
    if (m->name() == "main") {
        param_str = "int argc, char **argv";
    } else {
        for (auto param : m->params()) {
            std::string t = TypeSolver::native(param->varType());
            if (!param_str.empty()) param_str += ", ";
            param_str += t + " " + param->code();
        }
    }

    return ret + " " + m->name() + "(" + param_str +  ")";
}

bool Cgen::isAssignment(const std::string &s) const
{
    return s.find("==") == std::string::npos && s.find('=') != std::string::npos;
}

std::vector<std::string> Cgen::generateMethodAutoReturn(std::vector<std::string> lines, std::string ret)
{
    if (lines.empty() || ret == "void") return lines;

    std::string last = trim(lines.back());
    if (last.substr(0, 7) == "return ") return lines;

    lines.pop_back();
    // If not assignment, apply return
    if (!isAssignment(last)) last = "   return " + last;
    lines.push_back(last);

    return lines;
}

std::string Cgen::generateMethod(const PureMethod *m)
{
    std::string res;
    std::string proto = generateMethodPrototype(m);

    std::string ret = solveReturnType(m->returnType(), m);

    std::vector<std::string> lines;
    for (auto var : m->variables()) {
        for (auto l : generateVariable(var)) lines.push_back(l);
    }
    for (auto var : m->variables()) {
        lines.push_back(generateVariableInit(var));
    }

    for (auto block : m->blocks()) {
        for (auto l : generateBlock(block, ret, m)) lines.push_back(l);
    }

    lines = generateMethodAutoReturn(lines, ret);

    std::string body;
    for (auto l : lines) {
        body += l;
    }

    res += proto + " {\n";
    if (m->name() == "main") {
        // Take arguments into safe
        res += "__argc = argc;\n";
        res += "__argv = argv;\n";
    }
    res += body + "\n";
    res += "}\n";

    return res;
}

std::string Cgen::generateStructInitPrototype(const Struct *c) const
{
    return c->code() + "* new_" + c->code() + "()\n";
}

std::string Cgen::generateStructAlloc(const Struct *c) const
{
    // TODO Change to jemalloc or something else
    return c->code() + " *tmp = calloc(1, sizeof(" + c->code() + "));\n";
}

std::string Cgen::generateStructElementInitCall(const TypeIdent *i) const
{
    return indent() + "tmp->" + i->code() + " = new_" + i->varType() + "();\n";
}

std::string Cgen::generateStructElementInit(const Struct *c) const
{
    std::string res;
    for (auto i : c->datas()) {
        if (!TypeSolver::isNative(i->varType())) res += generateStructElementInitCall(i);
    }
    return res;
}

std::string Cgen::generateStructInitializer(const Struct *c)
{
    std::string res;
    res += generateStructInitPrototype(c);
    res += "{\n";
    res += indent() + generateStructAlloc(c);
    res += generateStructElementInit(c);
    res += indent() + "return tmp;\n";
    res += "}\n";
    return res;
}

std::string Cgen::generateStructHeader(const Struct *c) const
{
    return "typedef struct _" + c->code() + " ";
}

std::string Cgen::generateStructFooter(const Struct *c) const
{
    return c->code();
}

std::string Cgen::generateStructElements(const Struct *c) const
{
    std::string res;
    for (auto var : c->datas()) {
        for (auto l : generateVariable(var)) res += indent() + l;
    }
    return res;
}

void Cgen::addStruct(const Struct *c)
{
    m_structs.push_back(c);
}

std::string Cgen::generateStruct(const Struct *c)
{
    addStruct(c);
    return generateStructHeader(c) + "{\n" +
           generateStructElements(c) + "} " +
           generateStructFooter(c) + ";\n";
}

bool Cgen::isValidConst(const Const *c) const
{
    return c != nullptr && c->ident() != nullptr;
}

std::string Cgen::generateConstPrototype(const Const *c) const
{
    std::string t = TypeSolver::native(c->ident()->varType());
    return "const " + t + " " + c->ident()->code();
}

std::string Cgen::generateConstAssignment(const Const *c)
{
    const Assignment *ass = c->assignment();
    std::vector<std::string> tmp = StatementGenerator(this, ass->statements(), nullptr).generate();
    std::string res;
    for (auto i : tmp) res += i;
    return res;
}

std::string Cgen::generateConst(const Const *c)
{
    if (!isValidConst(c)) throw "Invalid const found!";
    return generateConstPrototype(c) + " = " +
           generateConstAssignment(c) + ";\n";
}

std::string Cgen::generateHeaders()
{
    std::string code;
    code += "#include <stddef.h>\n";
    code += "#include <stdlib.h>\n";
    code += "#include <stdint.h>\n";
    code += "typedef enum { false, true } boolean;\n";
    code += "static int __argc = 0;\n";
    code += "static char **__argv = NULL;\n";

    return code;
}

std::string Cgen::generateImports(const Compiler *c)
{
    std::string code = "\n/***** Imports **/\n";
    for (auto m : c->imports()) code += generateImport(m);
    return code;
}

std::string Cgen::generateStructs(const Compiler *c)
{
    std::string code = "\n/***** Structs **/\n";
    for (auto m : c->structs()) code += generateStruct(m);
    return code;
}

std::string Cgen::generateInitializers(const Compiler *c)
{
    std::string code = "\n/***** Struct Initializers **/\n";
    for (auto m : c->structs()) code += generateStructInitializer(m);
    return code;
}

std::string Cgen::generateConsts(const Compiler *c)
{
    std::string code = "\n/***** Consts **/\n";
    for (auto m : c->consts()) code += generateConst(m);
    return code;
}

std::string Cgen::generatePrototypes(const Compiler *c)
{
    std::string code = "\n/***** Prototypes **/\n";
    for (auto m : c->methods()) code += generateMethodPrototype(m.second) + ";\n";
    return code;
}

std::string Cgen::generateMethods(const Compiler *c)
{
    std::string code = "\n/***** Methods **/\n";
    for (auto m : c->methods()) code += generateMethod(m.second);
    return code;
}

std::string Cgen::generateUnit(const Compiler *c)
{
    std::string code;
    code += generateHeaders();
    code += generateImports(c);
    code += generateStructs(c);
    code += generateInitializers(c);
    code += generateConsts(c);
    code += generatePrototypes(c);
    code += generateMethods(c);

    return code;
}
