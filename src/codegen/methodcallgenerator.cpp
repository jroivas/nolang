#include "methodcallgenerator.hh"
#include "tools.hh"
#include "cgen.hh"
#include "statementgenerator.hh"
#include "typesolver.hh"
#include "zip.hh"

using namespace nolang;

MethodCallGenerator::MethodCallGenerator(Cgen * c, const MethodCall *pmc, const PureMethod *pm) :
    cgen(c),
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
        std::string t = TypeSolver(m).typeOfChain(parm);
        if (t.empty()) t = "void *";
        ptypes.push_back(t);
    }
}

void MethodCallGenerator::solveParameterNolangTypes()
{
    for (auto parm : mc->params()) {
        std::string nt = TypeSolver(m).nolangTypeOfChain(parm);
        nolang_ptypes.push_back(nt);
    }
}

void MethodCallGenerator::generateParameterStatements()
{
    // TODO Assert sizes
    uint32_t i = 0;
    for (auto parm : mc->params()) {
        parameter_statements.push_back(ptypes[i] + " " + pnames[i] + " = ");

        applyToVector(parameter_statements, StatementGenerator(cgen, parm, m).generate());
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
        if (sub == nullptr) throwError("Can't find from modules:" + def->name());
        mod = sub;
        ++idx;
    }
    // Got module, find method
    return mod->getMethod(def->values()[idx], nolang_ptypes);
}

std::vector<std::string> MethodCallGenerator::generateModuleMethodCallWithMethod(const ModuleMethodDef *meth) const
{
    std::vector<std::string> res;
    res.push_back(meth->fullName() + "(" + combineStringList(pnames) + ")");
    res.push_back("<EOS>");

    return res;
}

std::vector<std::string> MethodCallGenerator::generateModuleMethodCall(const ModuleDef *mod)
{
    const ModuleMethodDef *meth = getModuleMethodDef(mod);
    if (!meth) throwError("Can't find method from modules:" + def->name());
    return generateModuleMethodCallWithMethod(meth);
}

std::string MethodCallGenerator::generateBuiltInIOPrintParamTypes() const
{
    std::string res;
    for (auto v : ptypes) {
        if (v == "char *") res += "%s";
        else if (v == "const char *") res += "%s";
        else if (v == "int") res += "%d";
        else if (v == "uint8_t") res += "%hhu";
        else if (v == "uint16_t") res += "%hu";
        else if (v == "uint32_t") res += "%u";
        else if (v == "uint64_t") res += "%lu";
        else if (v == "int8_t") res += "%hhd";
        else if (v == "int16_t") res += "%hd";
        else if (v == "int32_t") res += "%d";
        else if (v == "int64_t") res += "%ld";
        // FIXME
        //else res += "%d";
    }
    return res;
}

std::string MethodCallGenerator::generateBuiltInIOPrint() const
{
    std::string tmp;
    tmp += "printf(\"";
    tmp += generateBuiltInIOPrintParamTypes();
    if (isPrintln()) tmp += "\\n";
    tmp += "\", ";
    tmp += combineStringList(pnames);
    tmp += ");";
    return tmp;
}

std::string MethodCallGenerator::generateLocalMethodCall() const
{
    std::string mname = combineStringList(mc->namespaces()->values(), ".");
    std::string params = "(" + combineStringList(pnames) + ")";
    return mname + params;
}

std::vector<std::string> MethodCallGenerator::generateStructInitStatements()
{
    std::vector<std::string> res;
    std::string postponed = cgen->usePostponedMethod();
    for (auto v : zip(cgen->getStruct(def->values()[0])->datas(), pnames)) {
        res.push_back(postponed + "->" + v.first->code() + " = " + v.second + ";\n");
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
    if (def == nullptr || def->values().empty()) throwError("Got empty namespace in method call");
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
