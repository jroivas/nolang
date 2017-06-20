#include "argumentparser.hh"
#include "tools.hh"

using namespace nolang;

ArgumentParser::ArgumentParser(Compiler *c, mpc_ast_t *t) :
    compiler(c),
    tree(t)
{
}

void ArgumentParser::reset()
{
    braceOpenLevel = 0;
    params.clear();
}

bool ArgumentParser::isOpenBrace() const
{
    return expect(item, "char", "(");
}

bool ArgumentParser::isCloseBrace() const
{
    return expect(item, "char", ")");
}

bool ArgumentParser::isParamDef() const
{
    return braceOpenLevel == 1 && expect(item, "paramdef");
}

bool ArgumentParser::isTypeIdent() const
{
    return braceOpenLevel == 1 && expect(item, "typeident");
}

bool ArgumentParser::isComma() const
{
    return braceOpenLevel == 1 && expect(item, "char", ",");
}

bool ArgumentParser::isTypeIdentifier(Statement *s) const
{
    return s && s->type() == "TypeIdent";
}

bool ArgumentParser::isOptionalWhitespace() const
{
    return expect(item, "ows");
}

void ArgumentParser::parseTypeIdent()
{
    auto res = compiler->codegen(item, nullptr, 0, true);
    for (auto r : res){
        if (isTypeIdentifier(r)) params.push_back(static_cast<TypeIdent*>(r));
        else throw std::string("Invalid parameter definition: " + r->code());
    }
}

void ArgumentParser::parseParamDef()
{
    mpc_ast_t *paramdef = item;
    iterateTree(paramdef, [&] (mpc_ast_t *def) {
        item = def;
        if (isTypeIdent()) parseTypeIdent();
        else if (isComma() || isOptionalWhitespace());
        else printError("Unknown node in parameter", item);
        /*
        std::string cnts = item->contents;
        if (expect(item, "typeident")) {
            m_parameters = true;
            auto res = codegen(item, m, level + 1);
            m_parameters = false;
            for (auto r : res){
                if (r->type() == "TypeIdent")
                    m->addParameter(static_cast<TypeIdent*>(r));
                else
                    throw std::string("Invalid parameter definition: " + r->code());
            }
        } else if (expect(item, "char") && cnts == ",") {
            // FIXME Can ',' separate in params something else than next param?
        } else printError("Unknown node in parameter", item);
        */
    });
}

void ArgumentParser::parseItem()
{
    if (isOpenBrace()) ++braceOpenLevel;
    else if (isCloseBrace()) --braceOpenLevel;
    else if (isParamDef()) parseParamDef();
    else printError("Unknown node in arguments", item);
}

std::vector<TypeIdent*> ArgumentParser::parse()
{
    reset();
    iterate(item, tree, parseItem);
    return params;
}
