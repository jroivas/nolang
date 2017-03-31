#include "cgen.hh"

#include <iostream>
#include <vector>

#include "trim.hh"

using namespace nolang;

std::string Cgen::solveNativeType(const TypeDef *t) const
{
    if (t->code() == "void") {
        return "void";
    }
    return "void *";
}

std::string Cgen::generateConst(Statement *)
{
    // FIXME
    return "";
}

std::string Cgen::generateImport(std::string imp)
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

std::string Cgen::generateStatement(Statement *s) const
{
    std::string res;
    
    if (s->type() == "String" ||
        s->type() == "Number" ||
        s->type() == "Op") {
        res += s->code() + " ";
    } else if (s->type() == "MethodCall") {
        MethodCall *mc = static_cast<MethodCall*>(s);

        // FIXME handle param gen
        for (auto parm : mc->params()) {
        }
        // FIXME Hardcoding
        if (mc->namespaces().size() == 2 &&
            mc->namespaces()[0] == "IO" &&
            mc->namespaces()[1] == "print") {
        }
    } else {
        std::cerr << "** ERROR: Unhandled statement: " << s->type() << " " << s->code() << "\n";
    }

    return res;
}

std::string Cgen::generateBlock(std::vector<std::vector<Statement *>> block, std::string ret) const
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
    if (ret != "void") {
        std::string last = lines.back();

        if (last.substr(0, 6) != "return ") {
            lines.pop_back();
            last = "return " + last;
            lines.push_back(last);
        }
    }

    std::string res;
    for (auto line : lines) {
        res += line + ";\n";
    }

    return res;
}

std::string Cgen::generateMethod(PureMethod *m)
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
