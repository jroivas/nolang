#pragma once

#include <string>

#include <3pp/mpc/mpc.h>

namespace nolang
{

class Parser {
public:
    Parser();
    ~Parser();

    std::string readFile(char *);
    mpc_result_t *parse(std::string name, std::string data);
    mpc_result_t *readParse(char *f) {
        return parse(f, readFile(f));
    }
    bool success() const {
        return m_success;
    }


protected:
    void init();
    void deinit();
    bool generateLang();
    bool m_ok;
    bool m_success;

    mpc_parser_t* Comment;
    mpc_parser_t* Indent;
    mpc_parser_t* Newline;
    mpc_parser_t* WhiteSpace;
    mpc_parser_t* OptionalWhiteSpace;
    mpc_parser_t* OptionalWhiteSpaceNewline;
    mpc_parser_t* Number;
    mpc_parser_t* Comparator;
    mpc_parser_t* BinaryOp;
    mpc_parser_t* FactorOperator;
    mpc_parser_t* TermOperator;
    mpc_parser_t* Expr;
    mpc_parser_t* Body;
    mpc_parser_t* Identifier;
    mpc_parser_t* Identcast;
    mpc_parser_t* TypeIdent;
    mpc_parser_t* Import;
    mpc_parser_t* Struct;
    mpc_parser_t* MethodRet;
    mpc_parser_t* MethodDef;
    mpc_parser_t* ParamDef;
    mpc_parser_t* LambdaParamDef;
    mpc_parser_t* String;
    mpc_parser_t* Args;
    mpc_parser_t* Lexp;
    mpc_parser_t* Factor;
    mpc_parser_t* Lambda;
    mpc_parser_t* MethodCall;
    mpc_parser_t* MapIndex;
    mpc_parser_t* ListItem;
    mpc_parser_t* MapItem;
    mpc_parser_t* TupleMap;
    mpc_parser_t* MapItems;
    mpc_parser_t* ListItems;
    mpc_parser_t* List;
    mpc_parser_t* Namespacedef;
    mpc_parser_t* Primitive;
    mpc_parser_t* Range;
    mpc_parser_t* MatchCond;
    mpc_parser_t* MatchCase;
    mpc_parser_t* Match;
    mpc_parser_t* If;
    mpc_parser_t* Assignment;
    mpc_parser_t* Par;
    mpc_parser_t* Stmt;
    mpc_parser_t* Term;
    mpc_parser_t* Const;
    mpc_parser_t* Return;
    mpc_parser_t* Pure;
    mpc_parser_t* TopLevel;
    mpc_parser_t* NolangPure;
};

}
