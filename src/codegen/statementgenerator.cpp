#include "statementgenerator.hh"

#include "methodcallgenerator.hh"

using namespace nolang;

void StatementGenerator::reset()
{
    lines.clear();
    statementcode.clear();
}

void StatementGenerator::generateString()
{
    statementcode = cgen->applyPostponed(statementcode);
    statementcode.push_back(statement->code() + " ");
}

void StatementGenerator::generateNumber()
{
    statementcode = cgen->applyPostponed(statementcode);
    statementcode.push_back(statement->code());
}

void StatementGenerator::generateBoolean()
{
    statementcode = cgen->applyPostponed(statementcode);
    statementcode.push_back(statement->code());
}

void StatementGenerator::generateBraces()
{
    statementcode = cgen->applyPostponed(statementcode);
    statementcode.push_back(statement->code() + " ");
}

void StatementGenerator::generateComparator()
{
    statementcode = cgen->applyPostponed(statementcode);
    statementcode.push_back(statement->code() + " ");
}

void StatementGenerator::generateOperator()
{
    std::string pp = statement->code();
    if (pp == "div") pp = "/";
    statementcode.push_back(pp + " ");
}

std::vector<std::string> StatementGenerator::generateMethodCall(const MethodCall *mc)
{
    MethodCallGenerator gen(cgen, mc, method);

    std::vector<std::string> res;
    if (gen.isStruct()) res = gen.generateStructInitCall();
    else res = gen.generateMethodCall();

    cgen->clearPostponed();

    return res;
}

void StatementGenerator::generateMethodCall()
{
    const MethodCall *mc = static_cast<const MethodCall*>(statement);
    for (auto l : generateMethodCall(mc)) statementcode.push_back(l);
}

bool StatementGenerator::isAssignmentMethodCall(const Assignment *ass) const
{
    for (auto sss : ass->statements()) {
        if (sss->type() == "MethodCall") return true;
    }
    return false;
}

void StatementGenerator::generateAssignment()
{
    const Assignment *ass = static_cast<const Assignment*>(statement);

    if (ass->def() != nullptr) {
        cgen->clearPostponed();
        for (auto s : StatementGenerator(cgen, ass->def(), method).generateOne()) {
            if (!cgen->isPostponed()) cgen->appendPostponed("->");
            cgen->appendPostponed(s);
        }
    } else {
        if (isAssignmentMethodCall(ass)) cgen->setPostponedMethod(statement->code());
        else cgen->setPostponed(statement->code());
    }
    
    //std::vector<std::string> tmp = generateStatements(ass->statements(), m);
    std::vector<std::string> tmp = StatementGenerator(cgen, ass->statements(), method).generate();
    for (auto s : tmp) std::cerr << " * SG " << s << "\n";
    statementcode = applyToVector(statementcode, tmp);
}

void StatementGenerator::generateNamespace()
{
}

void StatementGenerator::generateIdentfier()
{
    statementcode = cgen->applyPostponed(statementcode);
    statementcode.push_back(statement->code() + " ");
}

void StatementGenerator::generateEOS()
{
    statementcode.push_back("<EOS>");
}

void StatementGenerator::generateStatementCode()
{
    if (isString()) generateString();
    else if (isNumber()) generateNumber();
    else if (isBoolean()) generateBoolean();
    else if (isBraces()) generateBraces();
    else if (isComparator()) generateComparator();
    else if (isOperator()) generateOperator();
    else if (isMethodCall()) generateMethodCall();
    else if (isAssignment()) generateAssignment();
    else if (isNamespace()) generateNamespace();
    else if (isIdentifier()) generateIdentfier();
    else if (isEOS()) generateEOS();
    else printError("Unhandled statement", statement);
}

void StatementGenerator::generateStatement()
{
    statementcode.clear();
    generateStatementCode();
    if (!statementcode.empty()) lines = applyToVector(lines, statementcode);
}

void StatementGenerator::iterateStatements()
{
    for (auto s : stmts) {
        statement = s;
        generateStatement();
    }
}

std::vector<std::string> StatementGenerator::generate()
{
    reset();
    iterateStatements();
    return lines;
}
