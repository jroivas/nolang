#pragma once

#include "tools.hh"

namespace nolang {

class BaseParser
{
public:
    BaseParser() {}

protected:
    bool isRoot() const { return std::string(item->tag) == ">"; }
    bool isBody() const { return expect(item, "body"); }
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

    mpc_ast_t *item;
};

}
