#include "statementgenerator.hh"

#include "methodcallgenerator.hh"

using namespace nolang;

void StatementGenerator::reset()
{
    lines.clear();
    statementcode.clear();
}

std::vector<std::string> StatementGenerator::applyPostponed(std::vector<std::string> &res)
{
    if (!m_postponed_assignment.empty()) {
        res.push_back(m_postponed_assignment + " = ");
        m_postponed_assignment = "";
    }
    return res;
}

std::string StatementGenerator::usePostponedMethod()
{
    std::string tmp = m_postponed_method;
    m_postponed_method = "";
    return tmp;
}

void StatementGenerator::generateString()
{
    statementcode = applyPostponed(statementcode);
    statementcode.push_back(statement->code() + " ");
}

void StatementGenerator::generateNumber()
{
    statementcode = applyPostponed(statementcode);
    statementcode.push_back(statement->code());
}

void StatementGenerator::generateBoolean()
{
    statementcode = applyPostponed(statementcode);
    statementcode.push_back(statement->code());
}

void StatementGenerator::generateBraces()
{
    statementcode = applyPostponed(statementcode);
    statementcode.push_back(statement->code() + " ");
}

void StatementGenerator::generateComparator()
{
    statementcode = applyPostponed(statementcode);
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

    m_postponed_assignment = "";

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
        m_postponed_assignment = "";
        for (auto s : StatementGenerator(cgen, ass->def(), method).generateOne()) {
            if (!m_postponed_assignment.empty()) m_postponed_assignment += "->";
            m_postponed_assignment += s;
        }
    } else {
        if (isAssignmentMethodCall(ass)) m_postponed_method = statement->code();
        else m_postponed_assignment = statement->code();
    }
    
    //std::vector<std::string> tmp = generateStatements(ass->statements(), m);
    std::vector<std::string> tmp = StatementGenerator(cgen, ass->statements(), method).generate();
    statementcode = applyToVector(statementcode, tmp);
}

void StatementGenerator::generateNamespace()
{
}

void StatementGenerator::generateIdentfier()
{
    statementcode = applyPostponed(statementcode);
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
    generateStatementCode();
    if (!statementcode.empty()) lines = applyToVector(lines, statementcode);
}

void StatementGenerator::iterateStatements()
{
    statementcode.clear();
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
