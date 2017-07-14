#pragma once

#include "tools.hh"
#include "statement.hh"

namespace nolang {

template<class T>
class BaseParser
{
public:
    BaseParser(mpc_ast_t *t) :
        tree(t),
        item(nullptr),
        res(nullptr),
        parse_implemented(true)
    {
    }
    virtual ~BaseParser() {}
    virtual T *parse() final {
        doParse();
        return res;
    }

protected:
    // Logic must be implemented here
    virtual void reset() = 0;
    virtual void parseItem() = 0;
    virtual void postProcess() {}
    virtual bool isIterable() { return true; }
    virtual void nonIterableAction() {}

    bool isRoot() const { return std::string(item->tag) == ">"; }
    bool isBody() const { return expect(item, "body"); }
    bool isStatement() const { return expect(item, "stmt"); }
    bool isTerm() const { return expect(item, "term"); }
    bool isExpr() const { return expect(item, "expr"); }
    bool isLexp() const { return expect(item, "lexp"); }
    bool isComment() const { return expect(item, "comment"); }
    bool isMethodDef() const { return expect(item, "methoddef"); }
    bool isMethodCall() const { return expect(item, "methodcall"); }
    bool isStruct() const { return expect(item, "struct"); }
    bool isIndent() const { return expect(item, "indent"); }
    bool isAssignment() const { return expect(item, "assignment"); }
    bool isNumber() const { return expect(item, "number"); }
    bool isTermOp() const { return expect(item, "termop"); }
    bool isFactorOp() const { return expect(item, "factorop"); }
    bool isString() const { return expect(item, "string"); }
    bool isTypeIdent() const { return expect(item, "typeident"); }
    bool isNamespaceDef() const { return expect(item, "namespacedef"); }
    bool isImport() const { return expect(item, "import"); }
    bool isConst() const { return expect(item, "const"); }
    bool isComparator() const { return expect(item, "comparator"); }
    bool isNewLine() const { return expect(item, "newline"); }
    bool isCast() const { return expect(item, "string", "::"); }
    bool isDot() const { return expect(item, "char", "."); }
    bool isWhitespace() const { return expect(item, "ows") || expect(item, "ws") || expect(item, "regex", ""); }
    bool isOptionalWhitespace() const { return expect(item, "ows") || expect(item, "ws") || expect(item, "regex", ""); }
    bool isIdentifier() const { return expect(item, "identifier"); }
    bool isBrace() const { return expect(item, "char", "(") || expect(item, "char", ")"); }

    mpc_ast_t *tree;
    mpc_ast_t *item;

    T *res;

    bool parse_implemented;

private:
    void doParse();
};

template<class T>
void BaseParser<T>::doParse()
{
    if (!parse_implemented) throw std::string("Parse not implemented!");

    reset();

    if (isIterable()) iterate(item, tree, parseItem)
    else nonIterableAction();

    postProcess();
}

}
