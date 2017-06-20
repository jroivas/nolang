#include "methodparser.hh"
#include "tools.hh"

using namespace nolang;

MethodParser::MethodParser(Compiler *c, mpc_ast_t *t) :
    compiler(c),
    tree(t)
{
}

void MethodParser::reset()
{
    bool waitName = true;
    bool waitBody = false;
    method = new PureMethod();
}

bool MethodParser::isPure() const
{
    return waitName && expect(item, "pure")
}

bool MethodParser::isIdentifier() const
{
    return waitName && expect(item, "identifier")
}

bool MethodParser::isArguments() const
{
    return !waitName && expect(item, "args")
}

bool MethodParser::isMethodReturn() const
{
    return !waitName && expect(item, "methodret")
}

bool MethodParser::isOptionalWhitespace() const
{
    return expect(item, "ows")
}

bool MethodParser::isWhitespace() const
{
    return expect(item, "newline") || expect(item, "ws")
}

bool MethodParser::isBodyStart() const
{
    return !waitBody && expect(item, "string", "=>")
}

bool MethodParser::isBody() const
{
    return waitBody && expect(item, "body")
}

void MethodParser::parseIdentifier()
{
    method->setName(item->contents);
    waitName = false;
}

void MethodParser::parseArgs(mpc_ast_t *args)
{
    int open = 0;
    iterateTree(args, [&] (mpc_ast_t *item) {
        if (expect(item, "char")) {
            std::string cnts = item->contents;
            if (cnts == "(") open++;
            else if (cnts == ")") open--;
            else {
                throw std::string("Unexpected char: " + cnts);
            }
        } else if (open > 0 && expect(item, "paramdef"))
            parseParamDef(item, m, level + 1);
        else printError("Unknown node in arguments", item);
    });
}

void MethodParser::parseArguments()
{
    // FIXME
    parseArgs(item);
}

void MethodParser::parseMethodReturn()
{
    auto r = compiler->codegen(tree, method, 0);
    if (r.size() == 0) return
    if (r.size() > 1) {
        throw std::string("Expected one return type, got " + std::to_string(r.size()) + " for '" + method->name() + "'");
    }

    if (r[0]->type() != "Identifier") {
        throw std::string("Expected identifier as return type, got " + r[0]->type() + " for '" + method->name() + "'");
    }
    method->setReturnType(TypeDef(r[0]->code()));
}

void MethodParser::parseBodyStart()
{
    waitBody = true;
}

void MethodParser::parseBody()
{
    method->setBody(compiler->codegen(item, method, 0));
    // FIXME
    /*
    if (!m_blocks.empty()) {
        m->addBlock(m_blocks);
        m_blocks = std::vector<std::vector<Statement*>>();
    }
    */
}

void MethodParser::parseItem()
{
    if (isPure()) method->setPure();
    else if (isIdentifier()) parseIdentifier();
    else if (isArguments()) parseArguments();
    else if (isMethodReturn()) parseMethodReturn();
    else if (isOptionalWhitespace()) {}
    else if (isBodyStart()) parseBodyStart();
    else if (isBody()) parseBody();
    else if (isWhitespace()) {}
    else printError("Unknown node in method", item);
}

PureMethod *MethodParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return method;
}

