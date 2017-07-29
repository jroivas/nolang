#pragma once

#include "statement.hh"
#include "cgen.hh"

namespace nolang {

class StatementGenerator
{
public:
    StatementGenerator(Cgen *c, const std::vector<Statement *> s, const PureMethod *m) :
        cgen(c),
        stmts(s),
        method(m)
    {}
    std::vector<std::string> generate();
    std::string usePostponedMethod();
    std::vector<std::string> applyPostponed(std::vector<std::string> &);

protected:
    StatementGenerator(Cgen *c, const Statement * s, const PureMethod *m) :
        cgen(c),
        method(m),
        statement(s)
    {}
    std::vector<std::string> generateOne()
    {
        reset();
        generateStatement();
        return statementcode;
    }

private:
    Cgen *cgen;
    const std::vector<Statement *> stmts;
    const PureMethod *method;
    const Statement *statement;

    bool isString() { return statement->type() == "String"; }
    bool isNumber() { return statement->type() == "Number"; }
    bool isBoolean() { return statement->type() == "Boolean"; }
    bool isBraces() { return statement->type() == "Braces"; }
    bool isComparator() { return statement->type() == "Comparator"; }
    bool isOperator() { return statement->type() == "Op"; }
    bool isMethodCall() { return statement->type() == "MethodCall"; }
    bool isAssignment() { return statement->type() == "Assignment"; }
    bool isNamespace() { return statement->type() == "NamespaceDef"; }
    bool isIdentifier() { return statement->type() == "Identifier"; }
    bool isEOS() { return statement->type() == "EOS"; }
    bool isAssignmentMethodCall(const Assignment *) const;

    void generateString();
    void generateNumber();
    void generateBoolean();
    void generateBraces();
    void generateComparator();
    void generateOperator();
    void generateMethodCall();
    void generateAssignment();
    void generateNamespace();
    void generateIdentfier();
    void generateEOS();

    void generateStatementCode();
    void generateStatement();
    void iterateStatements();

    void reset();
    std::vector<std::string> generateMethodCall(const MethodCall *mc);

    std::vector<std::string> lines;
    std::vector<std::string> statementcode;

    std::string m_postponed_assignment;
    std::string m_postponed_method;
};

}
