#include <iostream>

#include <3pp/mpc.h>

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << "file\n";
        return 1;
    }
    mpc_parser_t* Comment   = mpc_new("comment");
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Body     = mpc_new("body");
    mpc_parser_t* Identifier = mpc_new("identifier");
    mpc_parser_t* TypeIdent = mpc_new("typeident");
    mpc_parser_t* Import = mpc_new("import");
    mpc_parser_t* MethodRet = mpc_new("methodret");
    mpc_parser_t* MethodDef = mpc_new("methoddef");
    mpc_parser_t* ParamDef = mpc_new("paramdef");
    mpc_parser_t* String = mpc_new("string");
    mpc_parser_t* Args = mpc_new("args");
    mpc_parser_t* Lexp = mpc_new("lexp");
    mpc_parser_t* Factor = mpc_new("factor");
    mpc_parser_t* MethodCall = mpc_new("methodcall");
    mpc_parser_t* MapIndex = mpc_new("mapindex");
    mpc_parser_t* ListItem = mpc_new("listitem");
    mpc_parser_t* MapItem = mpc_new("mapitem");
    mpc_parser_t* TupleMap = mpc_new("tuplemap");
    mpc_parser_t* MapItems = mpc_new("mapitems");
    mpc_parser_t* ListItems = mpc_new("listitems");
    mpc_parser_t* List = mpc_new("list");
    mpc_parser_t* Namespacedef = mpc_new("namespacedef");
    mpc_parser_t* MatchCase = mpc_new("matchcase");
    mpc_parser_t* Match = mpc_new("match");
    mpc_parser_t* Assignment = mpc_new("assignment");
    mpc_parser_t* Stmt = mpc_new("stmt");
    mpc_parser_t* Term = mpc_new("term");
    mpc_parser_t* Const = mpc_new("const");
    mpc_parser_t* Pure = mpc_new("pure");
    mpc_parser_t* NolangPure = mpc_new("nolangpure");

    mpc_err_t* err = mpca_lang(MPCA_LANG_DEFAULT,
        "comment    : /(#|\\/\\/)[^\\r\\n]*/ | /\\/\\*[^\\*]*(\\*[^\\/][^\\*]*)*\\*\\// ;"
        "identifier : /[A-Za-z_][A-Za-z0-9_-]*/ ;"
        "typeident  : <identifier> ':' <identifier> ;"
        "number     : /[0-9]+/ ;"
        "string     : /\"(\\\\.|[^\"])*\"/ | /\'(\\\\.|[^\'])*\'/ ; "
        "operator   : '+' | '-' | '*' | '/' ;                  "
        "import     : \"import\" <identifier> ;"
        "const      : \"const\" <assignment> ;"
        "methodret  : ':' <identifier> ;"
        "methoddef  : <pure>? <identifier> <args>? <methodret>? \"=>\" <body> ;"
        "paramdef   : <typeident>? (',' <typeident>)*; "
        "args       : '(' <paramdef>? ')'; "
        "factor     : '(' <lexp> ')' | <number> | <string> | <identifier>; "
        "term       : <factor> (('*' | '/' | \"div\" | \"mod\" | \"rem\") <factor>)*;"
        "lexp       : <term> (('+' | '-') <term>)* ; "
        "expr       : <term> | <factor> ;"
        "methodcall : '(' <lexp>? ')';"
        "mapindex   : '[' <lexp> ']';"
        "listitem   : <lexp>;"
        "mapitem    : <string> ':' <lexp> ;"
        "tuplemap   : '(' <string> ',' <lexp> ')';"
        //"list       : '[' (<listitem> (',' <listitem>)*) | (<mapitem> (',' <mapitem>)*) ']';"
        "mapitems   : (<tuplemap> | <mapitem>) (',' (<tuplemap> | <mapitem>)) *;"
        "listitems   : <listitem> (',' <listitem>) *;"
        "list       : '[' (<mapitems> | <listitems>) ?']';"
        "namespacedef : <identifier> ('.' <identifier>)+ (<methodcall> | <mapindex>)?;"
        "assignment : <typeident> '=' <stmt> ;"
        "matchcase  : (<identifier> | <number> | <string> | '?') ':' <stmt>;"
        "match      : \"match\" (<identifier> | <namespacedef>) \"=>\" <matchcase>+;"
        "stmt       : <match> | <assignment> | <namespacedef> | <lexp> | <list>;"
        "body       : (<stmt> | <comment>)* ;"
        "pure       : \"pure\" ;"
        "nolangpure : /^/ (<import> | <const> | <comment> | <methoddef>)* /$/ ;"
      ,
        Comment,
        Identifier, TypeIdent,
        Number, String,
        Operator,
        Import, Const,
        MethodRet, MethodDef, ParamDef, Args,
        Factor, Term, Lexp,
        Expr, MethodCall,
        MapIndex, ListItem, MapItem, TupleMap, MapItems, ListItems, List,
        Namespacedef,
        Assignment, MatchCase, Match, Stmt,
        Body, Pure,
        NolangPure, NULL);
    // expr       : <number> | '(' <operator> <expr>+ ')' ;  

    if (err != NULL) {
        mpc_err_print(err);
        mpc_err_delete(err);
        exit(1);
    }

    mpc_result_t r;
    if (mpc_parse_contents(argv[1], NolangPure, &r)) {
      /* On Success Print the AST */
      mpc_ast_print(static_cast<mpc_ast_t*>(r.output));
      mpc_ast_delete(static_cast<mpc_ast_t*>(r.output));
    } else {
      /* Otherwise Print the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    mpc_cleanup(15, 
        Identifier, TypeIdent, Number, String,
        Operator,
        MethodDef, ParamDef, Args,
        Factor, Term, Lexp,
        Expr, Body, Pure,
        NolangPure);

    return 0;
}
