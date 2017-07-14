#include "methodparser.hh"
#include "argumentparser.hh"
#include "tools.hh"

using namespace nolang;

MethodParser::MethodParser(Compiler *c, mpc_ast_t *t) :
    BaseParser(t),
    compiler(c)
{
}

void MethodParser::reset()
{
    waitName = true;
    waitBody = false;
    res = new PureMethod();
}

bool MethodParser::isPure() const
{
    return waitName && expect(item, "pure");
}

bool MethodParser::isIdentifier() const
{
    return waitName && expect(item, "identifier");
}

bool MethodParser::isArguments() const
{
    return !waitName && expect(item, "args");
}

bool MethodParser::isMethodReturn() const
{
    return !waitName && expect(item, "methodret");
}

bool MethodParser::isBodyStart() const
{
    return !waitBody && expect(item, "string", "=>");
}

bool MethodParser::isBody() const
{
    return waitBody && expect(item, "body");
}

void MethodParser::parseIdentifier()
{
    res->setName(item->contents);
    waitName = false;
}

bool MethodParser::isValidMethodReturn() const
{
    if (ret.size() > 1) throwError("Expected one return type, got", std::to_string(ret.size()) + " for", res->name());
    return ret.size() == 1;
}

bool MethodParser::isMethodReturnTypeIdentifier() const
{
    bool isOk = ret[0]->type() == "Identifier";
    if (!isOk) throwError("Expected identifier as return type, got ", ret[0]->type() + " for", res->name());
    return isOk;
}

void MethodParser::setMethodReturnType()
{
    res->setReturnType(TypeDef(ret[0]->code()));
}

void MethodParser::parseMethodReturn()
{
    ret = compiler->codegen(item, res, 0);
    if (isValidMethodReturn() && isMethodReturnTypeIdentifier()) setMethodReturnType();
}

void MethodParser::parseArguments()
{
    res->setParameters(ArgumentParser(compiler, item).parseList());
}

void MethodParser::parseBodyStart()
{
    waitBody = true;
}

void MethodParser::parseBody()
{
    res->setBody(compiler->codegen(item, res, 0));
    std::vector<std::vector<Statement*>> blocks = compiler->blocks();

    if (!blocks.empty()) {
        res->addBlock(blocks);
        compiler->clearBlocks();
    }
}

void MethodParser::parseItem()
{
    if (isPure()) res->setPure();
    else if (isIdentifier()) parseIdentifier();
    else if (isArguments()) parseArguments();
    else if (isMethodReturn()) parseMethodReturn();
    else if (isOptionalWhitespace());
    else if (isBodyStart()) parseBodyStart();
    else if (isBody()) parseBody();
    else if (isWhitespace() || isNewLine()) {}
    else printError("Unknown node in method", item);
}
