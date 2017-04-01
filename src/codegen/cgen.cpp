#include "cgen.hh"

#include <iostream>
#include <sstream>
#include <vector>

#include "trim.hh"

using namespace nolang;

Cgen::Cgen()
    : m_autogen_index(0),
      m_autogen_prefix("__autogen_")
{
}

std::string Cgen::autogen()
{
    std::stringstream ss;
    ss << m_autogen_prefix << ++m_autogen_index;
    return ss.str();
}

std::string Cgen::solveNativeType(const Statement *s) const
{
    if (s->type() == "TypeDef") {
        if (s->code() == "void") {
            return "void";
        }
        // FIXME
    }
    else if (s->type() == "String") {
        // FIXME "const char*" or "char*"
        return "char *";
    }
    else if (s->type() == "Number") {
        // FIXME type and size, floats
        return "long";
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

std::string Cgen::solveTypeOfChain(std::vector<Statement*> chain) const
{
    std::string res;
    for (auto c : chain) {
        std::string t = solveNativeType(c);
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

std::string Cgen::generateImport(const std::string &imp)
{
    std::string res;
    // FIXME Built-in import
    if (imp == "IO") {
       res += "#include <stdio.h>\n";
    } else {
        std::cerr << "** ERROR: Unhandled import" << imp << "\n";
    }
    return res;
}

std::string Cgen::generateMethodCall(const MethodCall *mc)
{
    // Strategy is to parse params first, and store result to temporary variables.
    std::vector<std::string> ptypes;
    std::vector<std::string> pnames;
    std::string res;
    for (auto parm : mc->params()) {
        std::string tmp;
        std::string pname = autogen();
        std::string t = solveTypeOfChain(parm);
        if (t.empty()) {
            t = "void *";
        }
        ptypes.push_back(t);
        pnames.push_back(pname);
        //tmp = t + " " + pname + ";\n";
        tmp = t + " " + pname + " = ";
        for (auto v : parm) {
            tmp += generateStatement(v);
        }
        tmp += ";\n";
        res += tmp;
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
        tmp += ");\n";
        res += tmp;
    }

    return res;
}

std::string Cgen::generateStatement(const Statement *s)
{
    std::string res;
    
    if (s->type() == "String" ||
        s->type() == "Number" ||
        s->type() == "Op") {
        res += s->code() + " ";
    } else if (s->type() == "MethodCall") {
        const MethodCall *mc = static_cast<const MethodCall*>(s);
        res += generateMethodCall(mc);
    } else {
        std::cerr << "** ERROR: Unhandled statement: " << s->type() << " " << s->code() << "\n";
    }

    return res;
}

std::string Cgen::generateBlock(const std::vector<std::vector<Statement *>> &block, const std::string &ret)
{
    std::vector<std::string> lines;
    for (auto line : block) {
        std::string tmp;
        for (auto stmt : line) {
            tmp += generateStatement(stmt);
        }
        tmp = trim(tmp);
        if (!tmp.empty()) {
            lines.push_back(tmp);
        }
    }

    if (lines.empty()) {
        return "";
    }
    // FIXME multiline block does not work with this
#if 0
    if (ret != "void") {
        std::string last = lines.back();

        if (last.substr(0, 6) != "return ") {
            lines.pop_back();
            last = "return " + last;
            lines.push_back(last);
        }
    }
#endif

    std::string res;
    for (auto line : lines) {
        res += line + ";\n";
    }

    return res;
}

std::string Cgen::generateMethod(const PureMethod *m)
{
    std::string res;

    std::string ret = solveNativeType(m->returnType());
#if 1
    // FIXME Forcing main to be "int"
    if (m->name() == "main") {
        // 
        if (ret == "void") ret = "int";
    }
#endif
    std::string body;
    for (auto block : m->blocks()) {
        body += generateBlock(block, ret);
    }

    res += ret + " " + m->name() + "() {\n";
    res += body + "\n";
    res += "}\n";

    return res;
}

std::string Cgen::generateUnit(const Compiler *c)
{
    std::string code;
    for (auto m : c->imports()) {
        code += generateImport(m);
    }
    for (auto m : c->methods()) {
        code += "\n/***** Method " + m.second->name() + " **/\n";
        code += generateMethod(m.second);
    }
    return code;
}
