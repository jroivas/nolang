#include <iostream>
#include <fstream>
#include <streambuf>

#include <3pp/mpc/mpc.h>

std::string readFile(char *fname)
{
    std::ifstream is(fname, std::ifstream::in);
    std::string str(
        (std::istreambuf_iterator<char>(is)),
        std::istreambuf_iterator<char>()
    );
    return str + '\n';
}

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << "file\n";
        return 1;
    }
    mpc_parser_t* Comment   = mpc_new("comment");
    mpc_parser_t* Indent   = mpc_new("indent");
    mpc_parser_t* Newline   = mpc_new("newline");
    mpc_parser_t* WhiteSpace = mpc_new("ws");
    mpc_parser_t* OptionalWhiteSpace = mpc_new("ows");
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* FactorOperator = mpc_new("factorop");
    mpc_parser_t* TermOperator = mpc_new("termop");
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
    mpc_parser_t* TopLevel = mpc_new("toplevel");
    mpc_parser_t* NolangPure = mpc_new("nolangpure");

    mpc_err_t* err = mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE,
        "comment    : /(#|\\/\\/)[^\\r\\n]*/"
        "           | /\\/\\*[^\\*]*(\\*[^\\/][^\\*]*)*\\*\\// ;"
        "indent     : ' ' ' '+ ;"
        "newline    : '\\n';"
        "ws         : /[' ']+/ ;"
        "ows        : /[' '\\t]*/ ;"
        "identifier : /[A-Za-z_][A-Za-z0-9_-]*/ ;"
        "typeident  : <identifier> <ows> ':' <ows> <identifier> ;"
        "number     : /[0-9]+/ ;"
        "string     : /\"(\\\\.|[^\"])*\"/ "
        "           | /\'(\\\\.|[^\'])*\'/ ; "
        "operator   : '+' | '-' | '*' | '/' ;"
        "factorop   : '*'"
        "           | '/'"
        "           | \"div\""
        "           | \"mod\""
        "           | \"rem\";"
        "termop     : '+'"
        "           | '-';"
        "import     : \"import\" <ws> <identifier> <newline>;"
        "const      : \"const\" <ws> <assignment> <newline>;"
        "methodret  : <ows> ':' <ows> <identifier> ;"
        "methoddef  : (<pure> <ws>)? <identifier> <ows> <args>? <methodret>? <ows> \"=>\" (<newline>|<ws>) <body> ;"
        "paramdef   : <typeident>? <ows> (',' <ows> <typeident>)*; "
        "args       : '(' <paramdef>? ')'; "
        "factor     : <namespacedef>"
        "           | '(' <lexp> ')'"
        "           | <number>"
        "           | <string>"
        "           | <identifier>; "
        "term       : <factor> (<ows> <factorop> <ows> <factor>)*;"
        "lexp       : <term> (<ows> <termop> <ows> <term>)* ; "
        "expr       : <list>"
        "           | <lexp>;"
        "methodcall : '(' (<expr> (',' <ows> <expr>)*)? ')';"
        "mapindex   : '[' <expr> ']';"
        "listitem   : <expr>;"
        "mapitem    : <string> <ows> ':' <ows> <expr> ;"
        "tuplemap   : '(' <string> <ows> ',' <ows> <lexp> ')';"
        "mapitems   : (<tuplemap> | <mapitem>) <ows> (',' <ows> (<tuplemap> | <mapitem>)) *;"
        "listitems  : <listitem> (<ows> ',' <ows> <listitem>) *;"
        "list       : '[' <ows> (<mapitems> | <listitems>)? <ows> ']' ;"
        "namespacedef : <identifier> ('.' <identifier>)* (<methodcall> | <mapindex>)?;"
        "assignment : <typeident> <ws> '=' <ws> <expr>;"
        "matchcase  : <indent> (<identifier> | <number> | <string> | '?') <ows> ':' <ows> <stmt> <newline>;"
        //"match      : \"match\" <ws> (<identifier> | <namespacedef>) <ows> \"=>\" <newline> <matchcase>+;"
        "match      : \"match\" <ws> <stmt> <ows> \"=>\" <newline> <matchcase>+;"
        "stmt       : <match>"
        "           | <assignment>"
        "           | <namespacedef>"
        "           | <list>"
        "           | <expr>"
        "           | <comment>;"
        "body       : ((<indent>+ <stmt>)? <newline>)* ;"
        "pure       : \"pure\" ;"
        "toplevel   : <import>"
        "           | <const>"
        "           | <comment>"
        "           | <methoddef>"
        "           | <newline>;"
        "nolangpure : /^/ <toplevel>* /$/;"
      ,
        Comment, Indent, Newline, WhiteSpace, OptionalWhiteSpace,
        Identifier, TypeIdent,
        Number, String,
        Operator, FactorOperator, TermOperator,
        Import, Const,
        MethodRet, MethodDef, ParamDef, Args,
        Factor, Term, Lexp,
        Expr, MethodCall,
        MapIndex, ListItem, MapItem, TupleMap, MapItems, ListItems, List,
        Namespacedef,
        Assignment, MatchCase, Match,
        Stmt,
        Body, Pure,
        TopLevel, NolangPure, NULL);

    if (err != NULL) {
        mpc_err_print(err);
        mpc_err_delete(err);
        exit(1);
    }

    mpc_result_t r;
    std::string data = readFile(argv[1]);
    if (mpc_parse(argv[1], data.c_str(), NolangPure, &r)) {
      mpc_ast_print(static_cast<mpc_ast_t*>(r.output));
      mpc_ast_delete(static_cast<mpc_ast_t*>(r.output));
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    mpc_cleanup(39,
        Comment, Indent, Newline, WhiteSpace, OptionalWhiteSpace,
        Identifier, TypeIdent,
        Number, String,
        Operator, FactorOperator, TermOperator,
        Import, Const,
        MethodRet, MethodDef, ParamDef, Args,
        Factor, Term, Lexp,
        Expr, MethodCall,
        MapIndex, ListItem, MapItem, TupleMap, MapItems, ListItems, List,
        Namespacedef,
        Assignment, MatchCase, Match,
        Stmt,
        Body, Pure,
        TopLevel, NolangPure);

    return 0;
}
