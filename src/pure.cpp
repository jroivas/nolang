#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <map>

#include <3pp/mpc/mpc.h>
#include "puremethod.hh"

std::string readFile(char *fname)
{
    std::ifstream is(fname, std::ifstream::in);
    std::string str(
        (std::istreambuf_iterator<char>(is)),
        std::istreambuf_iterator<char>()
    );
    return str + '\n';
}

int parsePure(std::string name, std::string data, mpc_result_t *r)
{
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
    mpc_parser_t* LambdaParamDef = mpc_new("lambdaparamdef");
    mpc_parser_t* String = mpc_new("string");
    mpc_parser_t* Args = mpc_new("args");
    mpc_parser_t* Lexp = mpc_new("lexp");
    mpc_parser_t* Factor = mpc_new("factor");
    mpc_parser_t* Lambda = mpc_new("lambda");
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
        "methodret  : <ows> ':' <ows> <identifier> ;"
        "methoddef  : (<pure> <ws>)? <identifier> <ows> <args>? <methodret>? <ows> \"=>\" (<newline>|<ws>) <body> ;"
        "paramdef   : <typeident>? <ows> (',' <ows> <typeident>)*; "
        "lambdaparamdef : (<identifier> (<ows> ':' <ows> <identifier>)?)? <ows> (',' <ows> <identifier> (<ows> ':' <ows> <identifier>)?)*; "
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
        "           | <lambda>"
        "           | <lexp> (<ows> <binaryop> <ows> <lexp>)*;"
        "lambda     : '(' <ows> <lambdaparamdef> <ows> \"=>\" ((<newline> <body>) | (<ows> <stmt>)) <ows> ')' ;"
        "methodcall : '(' (<stmt> (',' <ows> <stmt>)*)? ')';"
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
        "           | <expr>"
        "           | <namespacedef>"
        "           | <list>"
        "           | <comment>;"
        "body       : ((<indent>+ <stmt>)? <newline>)* ;"
        "pure       : \"pure\" | \"side\" ;"
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
        return 2;
    }

    int res = 0;
    if (mpc_parse(name.c_str(), data.c_str(), NolangPure, r)) {
        /*
        mpc_ast_print(static_cast<mpc_ast_t*>(r.output));
        mpc_ast_delete(static_cast<mpc_ast_t*>(r.output));
        */
    } else {
        /*
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
        */
        res = 1;
    }

    mpc_cleanup(39,
        Comment, Indent, Newline,
        WhiteSpace, OptionalWhiteSpace, OptionalWhiteSpaceNewline,
        Identifier, TypeIdent,
        Number, String,
        Comparator, BinaryOp, FactorOperator, TermOperator,
        Import, Const,
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

    return res;
}

std::string lvl(int l)
{
    std::string res = "";
    for (int i = 0; i < l; ++i) {
        res += " ";
    }
    return res;
}

void walk(mpc_ast_t *res, int l=0)
{
    std::string tmp = res->contents;
    if (tmp.length() > 0) {
        tmp = ": '" + tmp + '\'';
    }
    std::cout
        << "0x" << std::hex << long(res) << lvl(l)
        << " " << res->tag
        <<  tmp << "\n";
    for (int c = 0; c < res->children_num; ++c) {
        walk(res->children[c], l + 1);
    }
}

//std::string codegen(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::map<int, std::string> &blocks, int level=0);

std::string codegen(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::vector<std::string> &blocks, int level=0, PureMethod *m=nullptr);

//void parseMethod(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::map<int, std::string> &blocks, int level)
void parseMethod(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::vector<std::string> &blocks, int level)
{
    PureMethod *m = new PureMethod();
    bool waitName = true;
    bool waitBody = false;
    for (int c = 0; c < tree->children_num; ++c) {
        std::string tag = tree->children[c]->tag;
        std::string cnts = tree->children[c]->contents;
        if (waitName && tag.find("identifier") != std::string::npos) {
            m->name = cnts;
            waitName = false;
        } else if (tag.find("ows") != std::string::npos) {
            // Optional whitespace
        } else if (!waitBody && tag.find("string") != std::string::npos && cnts == "=>") {
            // Body should follow
            waitBody = true;
        } else if (waitBody && tag.find("body") != std::string::npos) {
            m->body = codegen(tree->children[c], methods, blocks, level, m);
        } else if (tag.find("newline") != std::string::npos) {
        //} else if (cnts.length() == 0) {
            // SKIP
        } else {
            std::cerr << "***ERROR: Unknown node: " << tag << ": '" << cnts << "'\n";
        }
    }
    methods[m->name] = m;
}

/*
void ensureBlockLevel(std::map<int, std::string> &blocks, int level)
{
    auto item = blocks.find(level);
    if (item == blocks.end()) {
        blocks[level] = "";
    }
}
*/

//std::string codegen(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::map<int, std::string> &blocks, int level)
std::string codegen(mpc_ast_t *tree, std::map<std::string, PureMethod*> &methods, std::vector<std::string> &blocks, int level, PureMethod *m)
{
    std::string res = "";

    std::string tag = tree->tag;
    std::string cnts = tree->contents;
    bool recurse = true;

    if (tag == ">") {
        //std::cout << "ROOT\n";
    } else if (tag.find("methoddef") != std::string::npos) {
        // New method
        parseMethod(tree, methods, blocks, level);
        recurse = false;
        level = 0;
    } else if (tag.find("indent") != std::string::npos) {
        int new_level = cnts.length();
        if (new_level != level) {
            if (m) {
                m->blocks.push_back(blocks);
                blocks = std::vector<std::string>();
            }
        }
        // FIME Blocks does not work this way
        //ensureBlockLevel(blocks, level);
        //std::cout << "LVL " << level << " '" << cnts << "'\n";
    } else if ((tag.find("regex") != std::string::npos && cnts.length() == 0) ||
               (tag.find("stmt") != std::string::npos && cnts.length() == 0) ||
               (tag.find("body") != std::string::npos && cnts.length() == 0)) {
        // SKIP
    } else if (tag.find("number") != std::string::npos) {
        res += cnts;
    } else if (tag.find("termop") != std::string::npos) {
        res += cnts;
    } else if (tag.find("newline") != std::string::npos) {
        // Commit?
        //ensureBlockLevel(blocks, level);
        blocks.push_back(res);
        res = "";
        //blocks[level].push_back(res);
    } else if (tag.find("ows") != std::string::npos) {
    } else {
        std::cerr << "***ERROR: Unknown node: " << tag << ": '" << cnts << "'\n";
    }

    if (recurse) {
        for (int c = 0; c < tree->children_num; ++c) {
            res += codegen(tree->children[c], methods, blocks, level);
        }
    }

    return res;
}

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << "file\n";
        return 1;
    }

    mpc_result_t r;
    std::string data = readFile(argv[1]);

    int res = parsePure(argv[1], data, &r);
    if (res == 2) {
        exit(res);
    }

    std::map<std::string, PureMethod*> methods;
    //std::map<int, std::string> blocks;
    std::vector<std::string> blocks;
    
    if (res == 0) {
        mpc_ast_print(static_cast<mpc_ast_t*>(r.output));
        //walk(static_cast<mpc_ast_t*>(r.output));
        std::cout << codegen(static_cast<mpc_ast_t*>(r.output), methods, blocks) << "\n";
        mpc_ast_delete(static_cast<mpc_ast_t*>(r.output));
        for (auto i : methods) {
            std::cout << i.first << ":\n";
            std::cout << i.second->body << "\n";
            for (auto v : i.second->blocks) {
                for (auto w : v) {
                    std::cout << "  " << w << ":\n";
                }
            }
        }
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }

    return res;
}
