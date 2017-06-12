#include "cgen.hh"

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <limits>

#include "trim.hh"

using namespace nolang;

Cgen::Cgen()
    : m_autogen_index(0),
      m_autogen_prefix("__autogen_"),
      m_current_module(nullptr)
{
}

std::string Cgen::autogen()
{
    std::stringstream ss;
    ss << m_autogen_prefix << ++m_autogen_index;
    return ss.str();
}

std::string Cgen::solveNativeType(const std::string & s) const
{
    // Defaulting "int" to int32
    if (s == "int32" || s == "int") {
        return "int32_t";
    } else if (s == "int8") {
        return "int8_t";
    } else if (s == "int16") {
        return "int16_t";
    } else if (s == "int64") {
        return "int64_t";
    } else if (s == "uint32") {
        return "uint32_t";
    } else if (s == "uint8") {
        return "uint8_t";
    } else if (s == "uint16") {
        return "uint16_t";
    } else if (s == "uint64") {
        return "uint64_t";
    } else if (s == "Double") {
        return "double";
    } else if (s == "f64") {
        return "double";
    } else if (s == "f32") {
        return "float";
    } else if (s == "boolean") {
        return "boolean";
    } else if (s == "string" || s == "String") {
        return "const char *";
    } else {
        return s + " *";
    }
    throw "Unknown native type: " + s;
}

bool Cgen::isNativeType(const std::string & s) const
{
    if (s.substr(0, 3) == "int" ||
        s.substr(0, 3) == "uint" ||
        s == "Double" ||
        s == "f64" ||
        s == "f32" ||
        s == "boolean" ||
        s == "string" ||
        s == "String") {
        return true;
    }
    return false;
}

TypeIdent *Cgen::solveVariable(const std::string &name, const PureMethod *m) const
{
    if (!m) return nullptr;
    for (auto var : m->variables()) {
        if (var->code() == name) {
            return var;
        }
    }
    return nullptr;
}

std::string Cgen::solveNativeType(const Statement *s, const PureMethod *m) const
{
    if (s->type() == "TypeDef") {
        if (s->code() == "void") {
            return "void";
        }
        return solveNativeType(s->code());
        // FIXME
    } else if (s->type() == "TypeIdent") {
        const TypeIdent *i = static_cast<const TypeIdent *>(s);
        std::string native = solveNativeType(i->varType());
        std::cout << "aSTT " << native << "\n";
    } else if (s->type() == "Identifier") {
        TypeIdent *var = solveVariable(s->code(), m);
        if (var) {
            return solveNativeType(var->varType());
            //std::cout << "STT " << native << "\n";
        }
        return "invalid";
    } else if (s->type() == "String" || s->type() == "string") {
        // FIXME "const char*" or "char*"
        return "char *";
    } else if (s->type() == "Number") {
        // FIXME type and size, floats
        std::string num = s->code();
        num = trim(num);
        /* TODO FIXME
        if (num.substr(0,2) == "0b") {
            // TODO Convert binary to hex
        }
        */
        double value = std::stod(num, nullptr);
        if (num[0] == '-') {
            if (fabs(value) >= std::numeric_limits<int32_t>::max()) {
                return "int64_t";
            }
            return "int32_t";
        } else {
            if (value >= std::numeric_limits<int32_t>::max()) {
                return "uint64_t";
            }
            return "uint32_t";
        }
    } else if (s->type() == "Boolean") {
        return "boolean";
    } else {
        throw std::string("Unknown native type: " + s->type());
    }
    #if 0
    //if (TypeDef *tdef = dynamic_cast<TypeDef*>(s)) {
    else if (StringValue *sval = dynamic_cast<StringValue*>(s)) {
        // FIXME "const char*" or "char*"
        return "char *";
    }
    else if (NumberValue *nval = dynamic_cast<NumberValue*>(s)) {
        // FIXME type and size, floats
        return "long";
    }
    #endif
    return "";
    //return "void *";
}

std::string Cgen::solveNolangType(const Statement *s, const PureMethod *m) const
{
    if (s->type() == "TypeDef") {
        if (s->code() == "void") {
            return "void";
        }
        return s->code();
        // FIXME
    } else if (s->type() == "TypeIdent") {
        const TypeIdent *i = static_cast<const TypeIdent *>(s);
        return i->varType();
    } else if (s->type() == "Identifier") {
        TypeIdent *var = solveVariable(s->code(), m);
        if (var) {
            var->varType();
        }
        return "invalid";
    } else if (s->type() == "String") {
        return "String";
    } else if (s->type() == "Number") {
        // FIXME type and size, floats
        return "int32";
    } else if (s->type() == "Boolean") {
        return "boolean";
    } else {
        throw std::string("Unknown type: " + s->type());
    }
    return "";
}

std::string Cgen::solveTypeOfChain(std::vector<Statement*> chain, const PureMethod *m) const
{
    std::string res;
    for (auto c : chain) {
        std::string t = solveNativeType(c, m);
        if (!t.empty()) {
            if (res.empty()) {
                res = t;
            } else if (res == t) {
                // OK
            } else {
                // Need to solve
                std::cerr << "*** ERROR: Can't solve type of chain, conflicting types: " << res << ", " << t << "\n";
            }
        }
    }
    return res;
}

std::string Cgen::solveNolangeTypeOfChain(std::vector<Statement*> chain, const PureMethod *m) const
{
    std::string res;
    for (auto c : chain) {
        std::string t = solveNolangType(c, m);
        if (!t.empty()) {
            if (res.empty()) {
                res = t;
            } else if (res == t) {
                // OK
            } else {
                // Need to solve
                std::cerr << "*** ERROR: Can't solve type of chain, conflicting types: " << res << ", " << t << "\n";
            }
        }
    }
    return res;
}

const ModuleDef *Cgen::getModule(std::string name) const
{
    auto mod = m_modules.find(name);
    if (mod != m_modules.end()) {
        return mod->second;
    }

    return nullptr;
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

std::vector<std::string> Cgen::generateStructInitializerCall(const NamespaceDef *def, const std::vector<std::string> &parameterNames, std::vector<std::string> &res)
{
    if (def->values().size() != 1) {
        throw std::string("Struct initializer with extra: " + def->name());
    }

    Struct *s = getStruct(def->values()[0]);
    uint32_t m = s->datas().size();
    if (m > parameterNames.size()) m = parameterNames.size();
    for (uint32_t i = 0; i < m; ++i) {
        std::string tmp = m_postponed_assignment + "->";
        tmp += s->datas()[i]->code();
        tmp += " = ";
        tmp += parameterNames[i++];
        tmp += ";\n";
        res.push_back(tmp);
    }
    m_postponed_assignment = "";
    return res;
}

std::vector<std::string> Cgen::solveParameterNames(const MethodCall *mc)
{
    std::vector<std::string> pnames;
    for (auto parm : mc->params()) {
        std::string pname = autogen();
        pnames.push_back(pname);
    }
    return pnames;
}

std::vector<std::string> Cgen::solveParameterTypes(const MethodCall *mc, const PureMethod *m)
{
    std::vector<std::string> ptypes;
    for (auto parm : mc->params()) {
        std::string t = solveTypeOfChain(parm, m);
        if (t.empty()) {
            // FIXME
            t = "void *";
        }
        ptypes.push_back(t);
    }
    return ptypes;
}

std::vector<std::string> Cgen::solveParameterNolangTypes(const MethodCall *mc, const PureMethod *m)
{
    std::vector<std::string> ptypes;
    for (auto parm : mc->params()) {
        std::string nt = solveNolangeTypeOfChain(parm, m);
        ptypes.push_back(nt);
    }
    return ptypes;
}

std::vector<std::string> Cgen::generateParameterStatements(const MethodCall *mc, const PureMethod *m, std::vector<std::string> ptypes, std::vector<std::string> pnames)
{
    std::vector<std::string> res;

    uint32_t i = 0;
    for (auto parm : mc->params()) {
        res.push_back(ptypes[i] + " " + pnames[i] + " = ");

        std::vector<std::string> tmp = generateStatements(parm, m);
        res.insert(res.end(), tmp.begin(), tmp.end());

        res.push_back("<EOS>");
        ++i;
    }

    return res;
}

const ModuleMethodDef *Cgen::getModuleMethodDef(const ModuleDef *mod, const NamespaceDef *def, const std::vector<std::string> &nolang_ptypes) const
{
    uint32_t idx = 1;
    // Next need to check namespace depth
    while (idx < def->values().size() - 1) {
        const ModuleDef *sub = mod->getModule(def->values()[idx]);
        if (sub == nullptr) {
            throw std::string("Can't find from modules:" + def->name());
        }
        mod = sub;
        ++idx;
    }
    // Got module, find method
    return mod->getMethod(def->values()[idx], nolang_ptypes);
}

std::vector<std::string> Cgen::generateModuleMethodCallWithMethod(const ModuleMethodDef *meth, const std::vector<std::string> &pnames) const
{
    std::vector<std::string> res;
    // FIXME Combine with below, create method
    std::string tmp;
    tmp += meth->fullName();
    tmp += "(";
    bool first = true;
    for (auto v : pnames) {
        if (!first) {
            tmp += ", ";
        }
        tmp += v;
        first = false;
    }
    tmp += ")";
    res.push_back(tmp);
    res.push_back("<EOS>");

    return res;
}

std::vector<std::string> Cgen::generateModuleMethodCall(const ModuleDef *mod, const NamespaceDef *def, const std::vector<std::string> &nolang_ptypes, const std::vector<std::string> &pnames)
{
    const ModuleMethodDef *meth = getModuleMethodDef(mod, def, nolang_ptypes);
    if (!meth) {
        throw std::string("Can't find method from modules:" + def->name());
    }
    return generateModuleMethodCallWithMethod(meth, pnames);
}

std::vector<std::string> Cgen::generateMethodCall(const MethodCall *mc, const PureMethod *m)
{
    // Strategy is to parse params first, and store result to temporary variables.
    std::vector<std::string> pnames = solveParameterNames(mc);
    std::vector<std::string> ptypes = solveParameterTypes(mc, m);
    std::vector<std::string> nolang_ptypes = solveParameterNolangTypes(mc, m);;
    std::vector<std::string> res = Cgen::generateParameterStatements(mc, m, ptypes, pnames);

    const NamespaceDef *def = mc->namespaces();
    if (def == nullptr || def->values().empty()) {
        throw std::string("Got empty namespace in method call");
    }

    if (isStruct(def->values()[0])) {
        return generateStructInitializerCall(def, pnames, res);
    }
    res = applyPostponed(res);
    const ModuleDef *mod = getModule(def->values()[0]);
    if (mod) {
        std::vector<std::string> tmp = generateModuleMethodCall(mod, def, nolang_ptypes, pnames);
        res.insert(res.end(), tmp.begin(), tmp.end());
    } else
    // FIXME Hardcoding
    if (mc->namespaces()->values().size() == 2 &&
        mc->namespaces()->values()[0] == "IO" &&
        (mc->namespaces()->values()[1] == "print" ||
         mc->namespaces()->values()[1] == "println")) {
        std::string tmp;
        tmp += "printf(\"";
        for (auto v : ptypes) {
            if (v == "char *") tmp += "\%s";
            else if (v == "const char *") tmp += "\%s";
            else if (v == "int") tmp += "\%d";
            else if (v == "uint8_t") tmp += "\%u";
            else if (v == "uint16_t") tmp += "\%u";
            else if (v == "uint32_t") tmp += "\%lu";
            else if (v == "uint64_t") tmp += "\%llu";
            else if (v == "int8_t") tmp += "\%d";
            else if (v == "int16_t") tmp += "\%d";
            else if (v == "int32_t") tmp += "\%ld";
            else if (v == "int64_t") tmp += "\%lld";
            // FIXME
            //else tmp += "\%d";
        }
        if (mc->namespaces()->values()[1] == "println") {
            tmp += "\\n";
        }
        tmp += "\", ";
        bool first = true;
        for (auto v : pnames) {
            if (!first) {
                tmp += ", ";
            }
            tmp += v;
            first = false;
        }
        tmp += ");";
        res.push_back(tmp);
        res.push_back("<EOS>");
    } else {
        std::string mname;
        for (std::string n : mc->namespaces()->values()) {
            if (!mname.empty()) mname += '.';
            mname += n;
        }
        std::string params = "(";
        bool first = true;
        for (auto v : pnames) {
            if (!first) {
                params += ", ";
            }
            params += v;
            first = false;
        }
        params += ")";
        res.push_back(mname + params);
        res.push_back("<EOS>");
    }

    return res;
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

std::vector<std::string> Cgen::generateStatement(const Statement *s, const PureMethod *m)
{
    std::vector<std::string> res;

    if (s->type() == "String") {
        //res = applyPostponed(res);
        res.push_back(s->code() + " ");
    } else if (s->type() == "Number") {
        //res = applyPostponed(res);
        res.push_back(s->code());
    } else if (s->type() == "Boolean") {
        //res = applyPostponed(res);
        res.push_back(s->code());
    } else if (s->type() == "Braces") {
        res = applyPostponed(res);
        res.push_back(s->code() + " ");
    } else if (s->type() == "Comparator") {
        //res = applyPostponed(res);
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
            m_postponed_assignment = s->code();
            //m_postponed_assignment = s->code() + " = ";
        }
        std::vector<std::string> tmp = generateStatements(ass->statements(), m);
        res.insert(res.end(), tmp.begin(), tmp.end());

    } else if (s->type() == "NamespaceDef") {
        const ModuleDef *mod = getModule(s->code());
        if (mod != nullptr) {
            std::cerr << "MODULE " << s->code() << "\n";
            m_current_module = mod;
        } else {
            const NamespaceDef *def = static_cast<const NamespaceDef *>(s);
            res = applyPostponed(res);
            if (!def->cast().empty()) {
                TypeIdent *st = solveVariable(def->code(), m);
                res.push_back(castCode(def->code(), st->varType(), def->cast()));
            } else if (!def->values().empty()) {
                // FIXME
                TypeIdent *st = solveVariable(def->code(), m);
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
    std::string native = solveNativeType(varType);

    res.push_back(native + " " + i->code() + ";\n");

    return res;
}

std::string Cgen::generateVariableInit(const TypeIdent *i)
{
    std::string varType = i->varType();
    if (!isNativeType(varType)) {
        return i->code() + " = new_" + varType + "();\n";
    }
    return "";
}

std::string Cgen::solveReturnType(const Statement *t, const PureMethod *m) const
{
    std::string ret = solveNativeType(m->returnType(), m);
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
            std::string t = solveNativeType(param->varType());
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
        if (!isNativeType(i->varType())) {
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
    std::string t = solveNativeType(c->ident()->varType());
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
