#include "assignmentparser.hh"
#include "tools.hh"

using namespace nolang;

AssignmentParser::AssignmentParser(Compiler *c, mpc_ast_t *t, PureMethod *m) :
    compiler(c),
    tree(t),
    method(m),
    assignment(nullptr)
{
}

void AssignmentParser::reset()
{
    wait_for_ident = true;
    wait_for_assign = false;
}

void AssignmentParser::gotIdentifier()
{
    wait_for_ident = false;
    wait_for_assign = true;
}

void AssignmentParser::gotAssignment()
{
    wait_for_assign = false;
}

void AssignmentParser::parseTypeIdent()
{
    TypeIdent *ident = compiler->parseTypeIdent(item, method);
    if (method) method->addVariable(ident);
    gotIdentifier();
    assignment = new Assignment(ident->code());
}

void AssignmentParser::parseNamespaceDef()
{
    NamespaceDef *def = compiler->parseNamespaceDef(item);
    if (def == nullptr)
        throw std::string("Invalid NamespaceDef in assignment");
    gotIdentifier();
    assignment = new Assignment(def);
}

void AssignmentParser::parseStatements()
{
    assignment->addStatements(compiler->codegen(item, method));
}

bool AssignmentParser::isTypeIdent() const
{
    return wait_for_ident && expect(item, "typeident");
}

bool AssignmentParser::isNamespaceDef() const
{
    return wait_for_ident && expect(item, "namespacedef");
}

bool AssignmentParser::isAssignment() const
{
    return wait_for_assign && expect(item, "char", "=");
}

bool AssignmentParser::isStatement() const
{
    return !wait_for_ident && !wait_for_assign;
}

void AssignmentParser::parseItem()
{
    if (isTypeIdent()) parseTypeIdent();
    else if (isNamespaceDef()) parseNamespaceDef();
    else if (isAssignment()) gotAssignment();
    else if (isStatement()) parseStatements();
    else printError("Unknown node in assignment", item);
}

Assignment *AssignmentParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return assignment;
}
