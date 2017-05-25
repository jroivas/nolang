#include "cgen.hh"

#include <iostream>
#include <sstream>
#include <vector>

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
    }
    throw "Unknown native type: " + s;
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
    } else if (s->type() == "String") {
        // FIXME "const char*" or "char*"
        return "char *";
    } else if (s->type() == "Number") {
        // FIXME type and size, floats
        return "long";
    } else {
        throw std::string("Unknown type: " + s->type());
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

std::string Cgen::generateConst(const Statement *)
{
    // FIXME
    return "";
}

ModuleDef *Cgen::getModule(std::string name)
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
    ModuleDef *mod = getModule(val);
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

std::vector<std::string> Cgen::generateMethodCall(const MethodCall *mc, const PureMethod *m)
{
    // Strategy is to parse params first, and store result to temporary variables.
    std::vector<std::string> ptypes;
    std::vector<std::string> pnames;
    std::vector<std::string> res;
    for (auto parm : mc->params()) {
        //std::string tmp;
        std::string pname = autogen();
        std::string t = solveTypeOfChain(parm, m);
        if (t.empty()) {
            // FIXME
            t = "void *";
        }
        ptypes.push_back(t);
        pnames.push_back(pname);
        //tmp = t + " " + pname + ";\n";
        //tmp = t + " " + pname + " = ";
        res.push_back(t + " " + pname + " = ");
        std::vector<std::string> tmp = generateStatements(parm, m);
        res.insert(res.end(), tmp.begin(), tmp.end());
        //tmp += ";\n";
        //res.push_back(";\n");
        res.push_back("<EOS>");
        //ptypes.push_back(tmp);
    }
    // FIXME Hardcoding
    if (mc->namespaces().size() == 2 &&
        mc->namespaces()[0] == "IO" &&
        (mc->namespaces()[1] == "print" ||
         mc->namespaces()[1] == "println")) {
        std::string tmp;
        tmp += "printf(\"";
        for (auto v : ptypes) {
            if (v == "char *") tmp += "\%s";
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
        if (mc->namespaces()[1] == "println") {
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
        for (std::string n : mc->namespaces()) {
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

std::vector<std::string> Cgen::generateStatement(const Statement *s, const PureMethod *m)
{
    std::vector<std::string> res;

    if (s->type() == "String" ||
        s->type() == "Number") {
        res.push_back(s->code() + " ");
    } else if (s->type() == "Op") {
        std::string pp = s->code();
        if (pp == "div") pp = "/";
        res.push_back(pp + " ");
        //res.push_back(s->code() + " ");
        //res += s->code() + " ";
    } else if (s->type() == "MethodCall") {
        const MethodCall *mc = static_cast<const MethodCall*>(s);
        for (auto l : generateMethodCall(mc, m)) {
            res.push_back(l);
        }
    } else if (s->type() == "Assignment") {
        res.push_back(s->code() + " = ");
        const Assignment *ass = static_cast<const Assignment*>(s);
        std::vector<std::string> tmp = generateStatements(ass->statements(), m);
        res.insert(res.end(), tmp.begin(), tmp.end());
    } else if (s->type() == "NamespaceDef") {
        //std::cerr << " NSD " << s->code() << "\n";
        ModuleDef *mod = getModule(s->code());
        if (mod != nullptr) {
            std::cerr << "MODULE " << s->code() << "\n";
            m_current_module = mod;
        } else {
            res.push_back(s->code() + " ");
        }
    } else if (s->type() == "Identifier") {
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

    std::string native = solveNativeType(i->varType());

    res.push_back(native + " " + i->code() +  ";\n");

    return res;
}

std::string Cgen::solveReturnType(const Statement *t, const PureMethod *m) const
{
    std::string ret = solveNativeType(m->returnType(), m);
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

    for (auto block : m->blocks()) {
        for (auto l : generateBlock(block, ret, m)) {
            lines.push_back(l);
        }
    }

    if (!lines.empty() && ret != "void") {
        std::string last = trim(lines.back());

        if (last.substr(0, 7) != "return ") {
            lines.pop_back();
            last = "   return " + last;
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

std::string Cgen::generateUnit(const Compiler *c)
{
    std::string code;
    code += "#include <stddef.h>\n";
    code += "#include <stdint.h>\n";
    code += "static int __argc = 0;\n";
    code += "static char **__argv = NULL;\n";

    code += "\n/***** Imports **/\n";
    for (auto m : c->imports()) {
        code += generateImport(m);
    }

    code += "\n/***** Globals **/\n";

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