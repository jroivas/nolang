#include "methodcallgenerator.hh"
#include "tools.hh"
#include "cgen.hh"

using namespace nolang;

MethodCallGenerator::MethodCallGenerator(Cgen * pcgen, const MethodCall *pmc, const PureMethod *pm) :
    cgen(pcgen),
    mc(pmc),
    m(pm)
{
    solveParameterNames();
    solveParameterTypes();
    solveParameterNolangTypes();
    generateParameterStatements();
    getNamespaceDef();
}

void MethodCallGenerator::solveParameterNames()
{
    for (auto parm : mc->params()) {
        std::string pname = cgen->autogen();
        pnames.push_back(pname);
    }
}

void MethodCallGenerator::solveParameterTypes()
{
    for (auto parm : mc->params()) {
        std::string t = cgen->solveTypeOfChain(parm, m);
        if (t.empty()) t = "void *";
        ptypes.push_back(t);
    }
}

void MethodCallGenerator::solveParameterNolangTypes()
{
    for (auto parm : mc->params()) {
        std::string nt = cgen->solveNolangTypeOfChain(parm, m);
        nolang_ptypes.push_back(nt);
    }
}

void MethodCallGenerator::generateParameterStatements()
{
    // TODO Assert sizes
    uint32_t i = 0;
    for (auto parm : mc->params()) {
        parameter_statements.push_back(ptypes[i] + " " + pnames[i] + " = ");

        std::vector<std::string> tmp = cgen->generateStatements(parm, m);
        applyToVector(parameter_statements, tmp);

        parameter_statements.push_back("<EOS>");
        ++i;
    }
}

const ModuleMethodDef *MethodCallGenerator::getModuleMethodDef(const ModuleDef *mod) const
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

std::vector<std::string> MethodCallGenerator::generateModuleMethodCallWithMethod(const ModuleMethodDef *meth) const
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

std::vector<std::string> MethodCallGenerator::generateModuleMethodCall(const ModuleDef *mod)
{
    const ModuleMethodDef *meth = getModuleMethodDef(mod);
    if (!meth) {
        throw std::string("Can't find method from modules:" + def->name());
    }
    return generateModuleMethodCallWithMethod(meth);
}

std::string MethodCallGenerator::generateBuiltInIOPrint() const
{
    std::string tmp;
    tmp += "printf(\"";
    for (auto v : ptypes) {
        if (v == "char *") tmp += "%s";
        else if (v == "const char *") tmp += "%s";
        else if (v == "int") tmp += "%d";
        else if (v == "uint8_t") tmp += "%hhu";
        else if (v == "uint16_t") tmp += "%hu";
        else if (v == "uint32_t") tmp += "%u";
        else if (v == "uint64_t") tmp += "%lu";
        else if (v == "int8_t") tmp += "%hhd";
        else if (v == "int16_t") tmp += "%hd";
        else if (v == "int32_t") tmp += "%d";
        else if (v == "int64_t") tmp += "%ld";
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
    return tmp;
}

std::string MethodCallGenerator::generateLocalMethodCall() const
{
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
    return mname + params;
}

std::vector<std::string> MethodCallGenerator::generateStructInitStatements()
{
    std::vector<std::string> res;
    Struct *s = cgen->getStruct(def->values()[0]);
    std::string postponed = cgen->usePostponedMethod();
    uint32_t m = s->datas().size();
    if (m > pnames.size()) m = pnames.size();
    for (uint32_t i = 0; i < m; ++i) {
        std::string tmp = postponed + "->";
        tmp += s->datas()[i]->code();
        tmp += " = ";
        tmp += pnames[i];
        tmp += ";\n";
        res.push_back(tmp);
    }
    return res;
}

std::vector<std::string> MethodCallGenerator::generateStructInitCall()
{
    if (def->values().size() != 1) {
        throw std::string("Struct initializer with extra: " + def->name());
    }

    std::vector<std::string> res;
    applyToVector(res, parameter_statements);
    applyToVector(res, generateStructInitStatements());

    return res;
}

void MethodCallGenerator::getNamespaceDef()
{
    def = mc->namespaces();
    if (def == nullptr || def->values().empty()) {
        throw std::string("Got empty namespace in method call");
    }
}

bool MethodCallGenerator::isStruct()
{
    return cgen->isStruct(def->values()[0]);
}

std::string MethodCallGenerator::getModuleName() const
{
    return def->values()[0];
}

std::vector<std::string> MethodCallGenerator::generateMethodCall()
{
    // Strategy is to parse params first, and store result to temporary variables.
    std::vector<std::string> res;
    applyToVector(res, parameter_statements);

    res = cgen->applyPostponed(res);
    const ModuleDef *mod = cgen->getModule(def->values()[0]);
    if (mod) {
        std::vector<std::string> tmp = generateModuleMethodCall(mod);
        applyToVector(res, tmp);
    } else
    // FIXME Hardcoding
    if (mc->namespaces()->values().size() == 2 &&
        mc->namespaces()->values()[0] == "IO" &&
        (mc->namespaces()->values()[1] == "print" ||
         mc->namespaces()->values()[1] == "println")) {

        res.push_back(generateBuiltInIOPrint());
        res.push_back("<EOS>");
    } else {
        res.push_back(generateLocalMethodCall());
        res.push_back("<EOS>");
    }

    return res;
}
