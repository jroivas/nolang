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
    mpc_parser_t* OptionalWhiteSpaceNewline = mpc_new("owsn");
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Comparator = mpc_new("comparator");
    mpc_parser_t* BinaryOp = mpc_new("binaryop");
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
    mpc_parser_t* Primitive = mpc_new("primitive");
    mpc_parser_t* Range = mpc_new("range");
    mpc_parser_t* MatchCond = mpc_new("matchcond");
    mpc_parser_t* MatchCase = mpc_new("matchcase");
    mpc_parser_t* Match = mpc_new("match");
    mpc_parser_t* If = mpc_new("if");
    mpc_parser_t* Assignment = mpc_new("assignment");
    mpc_parser_t* Par = mpc_new("par");
    mpc_parser_t* Stmt = mpc_new("stmt");
    mpc_parser_t* Term = mpc_new("term");
    mpc_parser_t* Const = mpc_new("const");
    mpc_parser_t* Return = mpc_new("return");
    mpc_parser_t* Pure = mpc_new("pure");
    mpc_parser_t* TopLevel = mpc_new("toplevel");
    mpc_parser_t* NolangPure = mpc_new("nolangpure");

    mpc_err_t* err = mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE,
        "comment    : /(#|\\/\\/)[^\\r\\n]*/"
        "           | /\\/\\*[^\\*]*(\\*[^\\/][^\\*]*)*\\*\\// ;"
        "indent     : /\\s\\s+/ ;"
        "newline    : '\\n';"
        "ws         : /[\\s]+/ ;"
        "ows        : /[\\s\\t]*/ ;"
        "owsn       : /[\\s\\t\\n]*/ ;"
        "identifier : /[A-Za-z_][A-Za-z0-9_-]*/ ;"
        "typeident  : <identifier> <ows> ':' <ows> <identifier> ;"
        "number     : /[0-9]+/ ;"
        "string     : /\"(\\\\.|[^\"])*\"/ "
        "           | /\'(\\\\.|[^\'])*\'/ ; "
        "comparator : \">=\""
        "           | \"<=\""
        "           | '>'"
        "           | '<'"
        "           | \"==\""
        "           | \"!=\";"
        "binaryop   : \"&&\""
        "           | \"||\""
        "           ;"
        "factorop   : '*'"
        "           | '/'"
        "           | \"div\""
        "           | \"mod\""
        "           | \"rem\";"
        "termop     : '+'"
        "           | '-';"
        "import     : \"import\" <ws> <namespacedef> (<ws> \"as\" <ws> <identifier>)?  <newline>;"
        "const      : \"const\" <ws> <assignment> <newline>;"
        "methodret  : <ows> ':' <ows> <identifier> ;"
        "methoddef  : (<pure> <ws>)? <identifier> <ows> <args>? <methodret>? <ows> \"=>\" (<newline>|<ws>) <body> ;"
        "paramdef   : <typeident>? <ows> (',' <ows> <typeident>)*; "
        "args       : '(' <paramdef>? ')'; "
        "factor     : <par>"
        "           | <namespacedef>"
        "           | '(' <lexp> ')'"
        "           | <number>"
        "           | <string>"
        "           | <identifier>; "
        "term       : <factor> (<ows> <factorop> <ows> <factor>)*;"
        "lexp       : <term> (<ows> (<termop> | <comparator>) <ows> <term>)* ; "
        "expr       : <list>"
        "           | <lexp> (<ows> <binaryop> <ows> <lexp>)*;"
        "methodcall : '(' (<expr> (',' <ows> <expr>)*)? ')';"
        "mapindex   : '[' <expr> ']';"
        "listitem   : <expr>;"
        "mapitem    : <string> <ows> ':' <owsn> <expr> ;"
        "tuplemap   : '(' <string> <owsn> ',' <owsn> <lexp> ')';"
        "mapitems   : (<tuplemap> | <mapitem>) <owsn> (',' <owsn> (<tuplemap> | <mapitem>)) *;"
        "listitems  : <listitem> (<owsn> ',' <owsn> <listitem>) *;"
        "list       : '[' <owsn> (<mapitems> | <listitems>)? <owsn> ']' ;"
        "namespacedef : <identifier> ('.' <identifier>)* (<methodcall> | <mapindex>)?;"
        "primitive  : <identifier> | <number> | <string> ;"
        "range      : <primitive> \"..\" <primitive>?;"
        "assignment : (<typeident>|<namespacedef>) <ws> '=' <ws> <expr>;"
        "matchcond  : <range> | ((<comparator> <ows>)? <primitive>) | '?';"
        "matchcase  : <comment> | (<indent> <matchcond> <ows> ':' <ows> <stmt> <newline>);"
        "match      : \"match\" <ws> <stmt> <ows> \"=>\" <newline> <matchcase>+;"
        "if         : \"if\" <ws> <stmt>"
        "             <ows> \"then\" <ws> <stmt>+"
        "             (<ows> \"else\" <ws> <stmt>+)?;"
        "par        : \"par\" <ws> <stmt>;"
        "stmt       : <match>"
        "           | <if>"
        "           | <return>"
        "           | <assignment>"
        "           | <expr>"
        "           | <namespacedef>"
        "           | <list>"
        "           | <comment>;"
        "body       : ((<indent>+ <stmt>)? <newline>)* ;"
        "pure       : \"pure\" ;"
        "return     : \"return\" <ws> <stmt> (<ows> ',' <ows> <stmt>)*;"
        "toplevel   : <import>"
        "           | <const>"
        "           | <comment>"
        "           | <methoddef>"
        "           | <newline>;"
        "nolangpure : /^/ <toplevel>* /$/;"
      ,
        Comment, Indent, Newline,
        WhiteSpace, OptionalWhiteSpace, OptionalWhiteSpaceNewline,
        Identifier, TypeIdent,
        Number, String,
        Comparator, BinaryOp, FactorOperator, TermOperator,
        Import, Const,
        MethodRet, MethodDef, ParamDef, Args,
        Factor, Term, Lexp,
        Expr, MethodCall,
        MapIndex, ListItem, MapItem, TupleMap, MapItems, ListItems, List,
        Namespacedef,
        Primitive, Range,
        Assignment,
        MatchCond, MatchCase, Match,
        If, Par,
        Stmt,
        Body, Pure, Return,
        TopLevel, NolangPure, NULL);

    if (err != NULL) {
        mpc_err_print(err);
        mpc_err_delete(err);
        exit(1);
    }

    mpc_result_t r;
    std::string data = readFile(argv[1]);
    int res = 0;
    if (mpc_parse(argv[1], data.c_str(), NolangPure, &r)) {
      mpc_ast_print(static_cast<mpc_ast_t*>(r.output));
      mpc_ast_delete(static_cast<mpc_ast_t*>(r.output));
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
        res = 1;
    }

    mpc_cleanup(39,
        Comment, Indent, Newline,
        WhiteSpace, OptionalWhiteSpace, OptionalWhiteSpaceNewline,
        Identifier, TypeIdent,
        Number, String,
        Comparator, BinaryOp, FactorOperator, TermOperator,
        Import, Const,
        MethodRet, MethodDef, ParamDef, Args,
        Factor, Term, Lexp,
        Expr, MethodCall,
        MapIndex, ListItem, MapItem, TupleMap, MapItems, ListItems, List,
        Namespacedef,
        Primitive, Range,
        Assignment,
        MatchCond, MatchCase, Match,
        If, Par,
        Stmt,
        Body, Pure, Return,
        TopLevel, NolangPure);

    return res;
}
