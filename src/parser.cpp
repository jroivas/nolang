#include "parser.hh"

#include <fstream>
#include <streambuf>

using namespace nolang;

Parser::Parser()
    : m_success(false)
{
    init();
}

Parser::~Parser()
{
    deinit();
}

void Parser::init()
{
    Comment   = mpc_new("comment");
    Indent   = mpc_new("indent");
    Newline   = mpc_new("newline");
    WhiteSpace = mpc_new("ws");
    OptionalWhiteSpace = mpc_new("ows");
    OptionalWhiteSpaceNewline = mpc_new("owsn");
    Number   = mpc_new("number");
    Comparator = mpc_new("comparator");
    BinaryOp = mpc_new("binaryop");
    FactorOperator = mpc_new("factorop");
    TermOperator = mpc_new("termop");
    Expr     = mpc_new("expr");
    Body     = mpc_new("body");
    Identifier = mpc_new("identifier");
    TypeIdent = mpc_new("typeident");
    Import = mpc_new("import");
    Struct = mpc_new("struct");
    MethodRet = mpc_new("methodret");
    MethodDef = mpc_new("methoddef");
    ParamDef = mpc_new("paramdef");
    LambdaParamDef = mpc_new("lambdaparamdef");
    String = mpc_new("string");
    Args = mpc_new("args");
    Lexp = mpc_new("lexp");
    Factor = mpc_new("factor");
    Lambda = mpc_new("lambda");
    MethodCall = mpc_new("methodcall");
    MapIndex = mpc_new("mapindex");
    ListItem = mpc_new("listitem");
    MapItem = mpc_new("mapitem");
    TupleMap = mpc_new("tuplemap");
    MapItems = mpc_new("mapitems");
    ListItems = mpc_new("listitems");
    List = mpc_new("list");
    Namespacedef = mpc_new("namespacedef");
    Primitive = mpc_new("primitive");
    Range = mpc_new("range");
    MatchCond = mpc_new("matchcond");
    MatchCase = mpc_new("matchcase");
    Match = mpc_new("match");
    If = mpc_new("if");
    Assignment = mpc_new("assignment");
    Par = mpc_new("par");
    Stmt = mpc_new("stmt");
    Term = mpc_new("term");
    Const = mpc_new("const");
    Return = mpc_new("return");
    Pure = mpc_new("pure");
    TopLevel = mpc_new("toplevel");
    NolangPure = mpc_new("nolangpure");

    m_ok = generateLang();
}

bool Parser::generateLang()
{
    mpc_err_t* err = mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE,
        "comment    : /(#|\\/\\/)[^\\r\\n]*/"
        "           | /\\/\\*[^\\*]*(\\*[^\\/][^\\*]*)*\\*\\// ;"
        "indent     : /\\s\\s+/ ;"
        "newline    : '\\n';"
        "ws         : /[\\s]+/ ;"
        "ows        : /[\\s\\t]*/ ;"
        "owsn       : /[\\s\\t\\n]*/ ;"
        "identifier : /[A-Za-z_][A-Za-z0-9_-]*/ (<ows> \"::\" <ows> <identifier>)?;"
        "typeident  : <identifier> <ows> ':' <ows> <identifier> ;"
        "number     : '-'? /[0-9]+/ ('.' /[0-9]+/)? ;"
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
        "struct     : \"struct\" <ws> <identifier> <ows> '{' (<ows> <typeident> <owsn>) * '}' <newline> ;"
        "methodret  : <ows> ':' <ows> <identifier> ;"
        "methoddef  : (<pure> <ws>)? <identifier> <ows> <args>? <methodret>? <ows> \"=>\" (<newline>|<ws>) <body> ;"
        "paramdef   : <typeident>? <ows> (',' <ows> <typeident>)*; "
        "lambdaparamdef : (<identifier> (<ows> ':' <ows> <identifier>)?)? <ows> (',' <ows> <identifier> (<ows> ':' <ows> <identifier>)?)*; "
        "args       : '(' <paramdef>? ')'; "
        "factor     : <par>"
        "           | <methodcall>"
        "           | <namespacedef>"
        "           | '(' <lexp> ')'"
        "           | <number>"
        "           | <string>"
        "           | <identifier>; "
        "term       : <factor> (<ows> <factorop> <ows> <factor>)*;"
        "lexp       : <term> (<ows> (<termop> | <comparator>) <ows> <term>)* ; "
        "expr       : <list>"
        "           | <lambda>"
        "           | <lexp> (<ows> <binaryop> <ows> <lexp>)*;"
        "lambda     : '(' <ows> <lambdaparamdef> <ows> \"=>\" ((<newline> <body>) | (<ows> <stmt>)) <ows> ')' ;"
        "methodcall : <namespacedef> <ows> '(' (<stmt> (',' <ows> <stmt>)*)? ')';"
        "mapindex   : '[' <expr> ']';"
        "listitem   : <expr>;"
        "mapitem    : <string> <ows> ':' <owsn> <expr> ;"
        "tuplemap   : '(' <string> <owsn> ',' <owsn> <lexp> ')';"
        "mapitems   : (<tuplemap> | <mapitem>) <owsn> (',' <owsn> (<tuplemap> | <mapitem>)) *;"
        "listitems  : <listitem> (<owsn> ',' <owsn> <listitem>) *;"
        "list       : '[' <owsn> (<mapitems> | <listitems>)? <owsn> ']' ;"
        "namespacedef : <identifier> ('.' <identifier>)* (<mapindex>)?;"
        "primitive  : <identifier> | <number> | <string> ;"
        "range      : <primitive> \"..\" <primitive>?;"
        "assignment : (<typeident>|<namespacedef> (<ows> ',' <ows> <namespacedef>)*) <ws> '=' <ws> <expr>;"
        "matchcond  : <range> | ((<comparator> <ows>)? <primitive>) | '?';"
        "matchcase  : <comment> | (<indent> <matchcond> <ows> ':' <ows> <stmt> <newline>);"
        "match      : \"match\" <ws> <stmt> <ows> \"=>\" <newline> <matchcase>+ (<indent> \"endmatch\")?;"
        "if         : \"if\" <ws> <stmt>"
        "             <ows> \"then\" <ws> <stmt>+"
        "             (<ows> \"else\" <ws> <stmt>+)?;"
        "par        : \"par\" <ws> <stmt>;"
        "stmt       : <match>"
        "           | <if>"
        "           | <return>"
        "           | <assignment>"
        "           | <typeident>"
        "           | <expr>"
        "           | <methodcall>"
        "           | <namespacedef>"
        "           | <list>"
        "           | <comment>;"
        "body       : ((<indent>+ <stmt>)? <newline>)* ;"
        "pure       : \"pure\" | \"side\" ;"
        "return     : \"return\" <ws> <stmt> (<ows> ',' <ows> <stmt>)*;"
        "toplevel   : <import>"
        "           | <const>"
        "           | <struct>"
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
        Import, Const, Struct,
        MethodRet, MethodDef, ParamDef, LambdaParamDef, Args,
        Factor, Term, Lexp,
        Expr, Lambda, MethodCall,
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
        return false;
    }

    return true;
}

void Parser::deinit()
{
    mpc_cleanup(39,
        Comment, Indent, Newline,
        WhiteSpace, OptionalWhiteSpace, OptionalWhiteSpaceNewline,
        Identifier, TypeIdent,
        Number, String,
        Comparator, BinaryOp, FactorOperator, TermOperator,
        Import, Const, Struct,
        MethodRet, MethodDef, ParamDef, LambdaParamDef, Args,
        Factor, Term, Lexp,
        Expr, Lambda, MethodCall,
        MapIndex, ListItem, MapItem, TupleMap, MapItems, ListItems, List,
        Namespacedef,
        Primitive, Range,
        Assignment,
        MatchCond, MatchCase, Match,
        If, Par,
        Stmt,
        Body, Pure, Return,
        TopLevel, NolangPure);
}

std::string Parser::readFile(char *fname)
{
    std::ifstream is(fname, std::ifstream::in);
    std::string str(
        (std::istreambuf_iterator<char>(is)),
        std::istreambuf_iterator<char>()
    );
    return str + '\n';
}

mpc_result_t *Parser::parse(std::string name, std::string data)
{
    mpc_result_t *res = new mpc_result_t;

    m_success = mpc_parse(name.c_str(), data.c_str(), NolangPure, res);

    return res;
}
