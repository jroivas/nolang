#include "cgen.hh"
#include "tools.hh"

#include <iostream>
#include <sstream>
#include <vector>

#include "typesolver.hh"
#include "trim.hh"

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

std::string Cgen::generateImport(const Import *imp)
{
    std::string res;
    // FIXME Built-in import
    std::string val = imp->val();
    const ModuleDef *mod = getModule(val);
    if (val == "IO") {
        // FIXME
        res += "#include <stdio.h>\n";
    } else if (mod != nullptr) {
        // FIXME
    } else {
        ModuleDef *def = new ModuleDef(val);
        if (def->ok()) {
            res += def->initCode();
            m_modules[val] = def;
        } else {
            delete def;
            res += "// FIXME: #include <" + val + ">";
            if (!imp->as().empty()) {
                res += " as " + imp->as();
            }
            res += "\n";
            std::cerr << "** ERROR: Unhandled import " << val << "\n";
        }
    }
    return res;
}

Struct *Cgen::getStruct(const std::string &name) const
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

std::string Cgen::usePostponedMethod()
{
    std::string tmp = m_postponed_method;
    m_postponed_method = "";
    return tmp;
}

std::vector<std::string> Cgen::generateMethodCall(const MethodCall *mc, const PureMethod *m)
{
    MethodCallGenerator gen(this, mc, m);
    std::vector<std::string> res;
    if (gen.isStruct()) {
        res = gen.generateStructInitCall();
    } else {
        res = gen.generateMethodCall();
    }
    m_postponed_assignment = "";
    return res;
}

bool Cgen::isAssignmentMethodCall(const Assignment *ass) const
{
    for (auto sss : ass->statements()) {
        if (sss->type() == "MethodCall") return true;
    }
    return false;
}

std::vector<std::string> Cgen::generateStatement(const Statement *s, const PureMethod *m)
{
    std::vector<std::string> res;

    if (s->type() == "String") {
        res = applyPostponed(res);
        res.push_back(s->code() + " ");
    } else if (s->type() == "Number") {
        res = applyPostponed(res);
        res.push_back(s->code());
    } else if (s->type() == "Boolean") {
        res = applyPostponed(res);
        res.push_back(s->code());
    } else if (s->type() == "Braces") {
        res = applyPostponed(res);
        res.push_back(s->code() + " ");
    } else if (s->type() == "Comparator") {
        res = applyPostponed(res);
        res.push_back(s->code() + " ");
    } else if (s->type() == "Op") {
        //res = applyPostponed(res);
        std::string pp = s->code();
        if (pp == "div") pp = "/";
        res.push_back(pp + " ");
    } else if (s->type() == "MethodCall") {
        const MethodCall *mc = static_cast<const MethodCall*>(s);
        for (auto l : generateMethodCall(mc, m)) {
            res.push_back(l);
        }
    } else if (s->type() == "Assignment") {
        const Assignment *ass = static_cast<const Assignment*>(s);

        if (ass->def() != nullptr) {
            m_postponed_assignment = "";
            for (auto s : generateStatement(ass->def(), m)) {
                if (!m_postponed_assignment.empty()) m_postponed_assignment += "->";
                m_postponed_assignment += s;
            }
            //m_postponed_assignment += "= ";
        } else {
            if (isAssignmentMethodCall(ass)) {
                m_postponed_method = s->code();
            } else {
                m_postponed_assignment = s->code();
            }
            //m_postponed_assignment = s->code() + " = ";
        }
        std::vector<std::string> tmp = generateStatements(ass->statements(), m);
        applyToVector(res, tmp);

    } else if (s->type() == "NamespaceDef") {
        const ModuleDef *mod = getModule(s->code());
        if (mod != nullptr) {
            std::cerr << "MODULE " << s->code() << "\n";
            m_current_module = mod;
        } else {
            const NamespaceDef *def = static_cast<const NamespaceDef *>(s);
            res = applyPostponed(res);
            if (!def->cast().empty()) {
                TypeIdent *st = TypeSolver(m).solveVariable(def->code());
                if (st == nullptr) {
                    throw "Invalid type identifier: " + def->code();
                }
                res.push_back(castCode(def->code(), st->varType(), def->cast()));
            } else if (!def->values().empty()) {
                // FIXME
                TypeIdent *st = TypeSolver(m).solveVariable(def->code());
                std::string tmp;
                for (auto v : def->values()) {
                    if (!tmp.empty()) tmp += "->";
                    tmp += v;
                }
                tmp += " ";
                res.push_back(tmp);
            } else {
                res.push_back(s->code() + " ");
            }
        }
    } else if (s->type() == "Identifier") {
        res = applyPostponed(res);
        res.push_back(s->code() + " ");
    } else if (s->type() == "EOS") {
        res.push_back("<EOS>");
    } else {
        std::cerr << "** ERROR: Unhandled statement: " << s->type() << " " << s->code() << "\n";
    }

    return res;
}

std::vector<std::string> Cgen::generateStatements(const std::vector<Statement *> stmts, const PureMethod *m)
{
    std::vector<std::string> lines;
    for (auto stmt : stmts) {
        for (auto l : generateStatement(stmt, m)) {
            //l = trim(l);
            if (!l.empty()) {
                lines.push_back(l);
            }
        }
    }
    return lines;
}

std::vector<std::string> Cgen::generateBlock(const std::vector<std::vector<Statement *>> &block, const std::string &ret, const PureMethod *m)
{
    std::vector<std::string> lines;
    for (auto line : block) {
        std::vector<std::string> tmp = generateStatements(line, m);
        //tmp = trim(tmp);
        if (!tmp.empty()) {
            std::string resline;
            for (auto ll : tmp) {
                if (ll == "<EOS>") {
                    resline = trim(resline);
                    if (!resline.empty()) {
                        lines.push_back(resline + ";\n");
                    }
                    resline = "";
                } else {
                    resline += ll;
                    //lines.push_back(ll);
                }
            }
            if (!resline.empty()) {
                lines.push_back(resline + ";\n");
            }
        }
    }

    // FIXME multiline block does not work with this
#if 0
    if (!lines.empty() && ret != "void") {
        std::string last = lines.back();

        if (last.substr(0, 6) != "return ") {
            lines.pop_back();
            last = "return " + last;
            lines.push_back(last);
        }
    }
#endif

#if 0
    std::string res;
    for (auto line : lines) {
        res += line + ";\n";
    }

    return res;
#endif
    return lines;
}

std::vector<std::string> Cgen::generateVariable(const TypeIdent *i)
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

std::string Cgen::generateMethod(const PureMethod *m)
{
    std::string res;
    std::string proto = generateMethodPrototype(m);

    std::string ret = solveReturnType(m->returnType(), m);

    std::vector<std::string> lines;
    for (auto var : m->variables()) {
        for (auto l : generateVariable(var)) {
            lines.push_back(l);
        }
    }
    for (auto var : m->variables()) {
        lines.push_back(generateVariableInit(var));
    }

    for (auto block : m->blocks()) {
        for (auto l : generateBlock(block, ret, m)) {
            lines.push_back(l);
        }
    }

    if (!lines.empty() && ret != "void") {
        std::string last = trim(lines.back());

        if (last.substr(0, 7) != "return ") {
            lines.pop_back();
            // No on assignment
            if (last.find("==") != std::string::npos ||
                last.find('=') == std::string::npos) {
                last = "   return " + last;
            }
            lines.push_back(last);
        }
    }

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

std::string Cgen::generateStructInitializer(const Struct *c)
{
    std::string res;
    res += c->code() + "* new_" + c->code() + "()\n";
    res += "{\n";
    // TODO Change to jemalloc or something else
    res += "    " + c->code() + " *tmp = calloc(1, sizeof(" + c->code() + "));\n";
    for (auto i : c->datas()) {
        if (!TypeSolver::isNative(i->varType())) {
            res += "    tmp->" + i->code() + " = new_" + i->varType() + "();\n";
        }
    }
    res += "    return tmp;\n";
    res += "}\n";
    return res;
}

std::string Cgen::generateStruct(const Struct *c)
{
    std::string res;
    res += "typedef struct _" + c->code() + " {\n";
    for (auto var : c->datas()) {
        for (auto l : generateVariable(var)) {
            res += l;
        }
    }
    res += "} " + c->code() + ";\n";
    return res;
}

std::string Cgen::generateConst(const Const *c)
{
    std::string res;
    if (c == nullptr || c->ident() == nullptr) {
        throw "Invalid const found!";
    }
    std::string t = TypeSolver::native(c->ident()->varType());
    res += "const " + t + " " + c->ident()->code();
    res += " = ";
    const Assignment *ass = c->assignment();
    std::vector<std::string> tmp = generateStatements(ass->statements(), nullptr);
    for (auto i : tmp) {
        res += i;
    }
    res += ";\n";
    return res;
}

std::string Cgen::generateUnit(const Compiler *c)
{
    std::string code;
    code += "#include <stddef.h>\n";
    code += "#include <stdlib.h>\n";
    code += "#include <stdint.h>\n";
    code += "typedef enum { false, true } boolean;\n";
    code += "static int __argc = 0;\n";
    code += "static char **__argv = NULL;\n";

    code += "\n/***** Imports **/\n";
    for (auto m : c->imports()) {
        code += generateImport(m);
    }

    code += "\n/***** Structs **/\n";
    for (auto m : c->structs()) {
        code += generateStruct(m);
        m_structs.push_back(m);
    }

    code += "\n/***** Struct Initializers **/\n";
    for (auto m : c->structs()) {
        code += generateStructInitializer(m);
    }

    code += "\n/***** Consts **/\n";
    for (auto m : c->consts()) {
        code += generateConst(m);
    }

    code += "\n/***** Prototypes **/\n";
    for (auto m : c->methods()) {
        code += generateMethodPrototype(m.second) + ";\n";
    }
    code += "\n/***** Methods **/\n";
    for (auto m : c->methods()) {
        code += generateMethod(m.second);
    }
    return code;
}
