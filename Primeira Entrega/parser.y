%{
#include "scanner.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

static Token current_token;
static int lexical_errors;
static int syntax_errors;
static AstNode *parse_tree;

static int yylex(void);
static void yyerror(const char *message);
%}

%locations
%define parse.error detailed
%define parse.lac full
%expect 0

%union {
  AstNode *node;
}

%token <node> ELSE "else" IF "if" INT "int" RETURN "return"
%token <node> VOID "void" WHILE "while"
%token <node> LE "<=" GE ">=" EQ "==" NE "!="
%token <node> PLUS "+" MINUS "-" TIMES "*" OVER "/"
%token <node> LT "<" GT ">" ASSIGN "="
%token <node> SEMI ";" COMMA ","
%token <node> LPAREN "(" RPAREN ")"
%token <node> LBRACKET "[" RBRACKET "]"
%token <node> LBRACE "{" RBRACE "}"
%token <node> ID "identifier" NUM "number"

%type <node> program declaration_list declaration var_declaration
%type <node> type_specifier fun_declaration params param_list param
%type <node> compound_stmt local_declarations statement_list statement
%type <node> expression_stmt selection_stmt iteration_stmt return_stmt
%type <node> expression var simple_expression relop additive_expression
%type <node> addop term mulop factor call args arg_list

%destructor { ast_free($$); } <node>

%precedence IF_WITHOUT_ELSE
%precedence ELSE

%start program

%%

program:
    declaration_list { parse_tree = ast_new("program", 1, $1); $$ = NULL; }
;

declaration_list:
    declaration { $$ = ast_new("declaration_list", 1, $1); }
  | declaration_list declaration
      { $$ = ast_new("declaration_list", 2, $1, $2); }
;

declaration:
    var_declaration { $$ = ast_new("declaration", 1, $1); }
  | fun_declaration { $$ = ast_new("declaration", 1, $1); }
;

var_declaration:
    type_specifier ID SEMI
      { $$ = ast_new("var_declaration", 3, $1, $2, $3); }
  | type_specifier ID LBRACKET NUM RBRACKET SEMI
      { $$ = ast_new("var_declaration", 6, $1, $2, $3, $4, $5, $6); }
;

type_specifier:
    INT { $$ = ast_new("type_specifier", 1, $1); }
  | VOID { $$ = ast_new("type_specifier", 1, $1); }
;

fun_declaration:
    type_specifier ID LPAREN params RPAREN compound_stmt
      { $$ = ast_new("fun_declaration", 6, $1, $2, $3, $4, $5, $6); }
;

params:
    param_list { $$ = ast_new("params", 1, $1); }
  | VOID { $$ = ast_new("params", 1, $1); }
;

param_list:
    param { $$ = ast_new("param_list", 1, $1); }
  | param_list COMMA param
      { $$ = ast_new("param_list", 3, $1, $2, $3); }
;

param:
    type_specifier ID { $$ = ast_new("param", 2, $1, $2); }
  | type_specifier ID LBRACKET RBRACKET
      { $$ = ast_new("param", 4, $1, $2, $3, $4); }
;

compound_stmt:
    LBRACE local_declarations statement_list RBRACE
      { $$ = ast_new("compound_stmt", 4, $1, $2, $3, $4); }
;

local_declarations:
    %empty { $$ = ast_new("local_declarations", 1, ast_empty()); }
  | local_declarations var_declaration
      { $$ = ast_new("local_declarations", 2, $1, $2); }
;

statement_list:
    %empty { $$ = ast_new("statement_list", 1, ast_empty()); }
  | statement_list statement
      { $$ = ast_new("statement_list", 2, $1, $2); }
;

statement:
    expression_stmt { $$ = ast_new("statement", 1, $1); }
  | compound_stmt { $$ = ast_new("statement", 1, $1); }
  | selection_stmt { $$ = ast_new("statement", 1, $1); }
  | iteration_stmt { $$ = ast_new("statement", 1, $1); }
  | return_stmt { $$ = ast_new("statement", 1, $1); }
;

expression_stmt:
    expression SEMI { $$ = ast_new("expression_stmt", 2, $1, $2); }
  | SEMI { $$ = ast_new("expression_stmt", 1, $1); }
;

selection_stmt:
    IF LPAREN expression RPAREN statement %prec IF_WITHOUT_ELSE
      { $$ = ast_new("selection_stmt", 5, $1, $2, $3, $4, $5); }
  | IF LPAREN expression RPAREN statement ELSE statement
      { $$ = ast_new("selection_stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
;

iteration_stmt:
    WHILE LPAREN expression RPAREN statement
      { $$ = ast_new("iteration_stmt", 5, $1, $2, $3, $4, $5); }
;

return_stmt:
    RETURN SEMI { $$ = ast_new("return_stmt", 2, $1, $2); }
  | RETURN expression SEMI
      { $$ = ast_new("return_stmt", 3, $1, $2, $3); }
;

expression:
    var ASSIGN expression { $$ = ast_new("expression", 3, $1, $2, $3); }
  | simple_expression { $$ = ast_new("expression", 1, $1); }
;

var:
    ID { $$ = ast_new("var", 1, $1); }
  | ID LBRACKET expression RBRACKET
      { $$ = ast_new("var", 4, $1, $2, $3, $4); }
;

simple_expression:
    additive_expression relop additive_expression
      { $$ = ast_new("simple_expression", 3, $1, $2, $3); }
  | additive_expression { $$ = ast_new("simple_expression", 1, $1); }
;

relop:
    LE { $$ = ast_new("relop", 1, $1); }
  | LT { $$ = ast_new("relop", 1, $1); }
  | GT { $$ = ast_new("relop", 1, $1); }
  | GE { $$ = ast_new("relop", 1, $1); }
  | EQ { $$ = ast_new("relop", 1, $1); }
  | NE { $$ = ast_new("relop", 1, $1); }
;

additive_expression:
    additive_expression addop term
      { $$ = ast_new("additive_expression", 3, $1, $2, $3); }
  | term { $$ = ast_new("additive_expression", 1, $1); }
;

addop:
    PLUS { $$ = ast_new("addop", 1, $1); }
  | MINUS { $$ = ast_new("addop", 1, $1); }
;

term:
    term mulop factor
      { $$ = ast_new("term", 3, $1, $2, $3); }
  | factor { $$ = ast_new("term", 1, $1); }
;

mulop:
    TIMES { $$ = ast_new("mulop", 1, $1); }
  | OVER { $$ = ast_new("mulop", 1, $1); }
;

factor:
    LPAREN expression RPAREN
      { $$ = ast_new("factor", 3, $1, $2, $3); }
  | var { $$ = ast_new("factor", 1, $1); }
  | call { $$ = ast_new("factor", 1, $1); }
  | NUM { $$ = ast_new("factor", 1, $1); }
;

call:
    ID LPAREN args RPAREN { $$ = ast_new("call", 4, $1, $2, $3, $4); }
;

args:
    %empty { $$ = ast_new("args", 1, ast_empty()); }
  | arg_list { $$ = ast_new("args", 1, $1); }
;

arg_list:
    expression { $$ = ast_new("arg_list", 1, $1); }
  | arg_list COMMA expression
      { $$ = ast_new("arg_list", 3, $1, $2, $3); }
;

%%

static int yylex(void) {
    TokenKind kind;
    int token;

    for (;;) {
        kind = lexer_next_token(&current_token);

        yylloc.first_line = current_token.line;
        yylloc.last_line = current_token.line;
        yylloc.first_column = current_token.column;
        yylloc.last_column = current_token.column +
                             (current_token.length ? (int)current_token.length - 1 : 0);

        if (kind == TOKEN_ERROR) {
            fprintf(stderr, "Erro lexico na linha %d, coluna %d: %s\n",
                    current_token.line, current_token.column,
                    current_token.error_message);
            lexical_errors++;
            continue;
        }
        break;
    }

    switch (kind) {
    case TOKEN_EOF: return 0;
    case TOKEN_ELSE: token = ELSE; break;
    case TOKEN_IF: token = IF; break;
    case TOKEN_INT: token = INT; break;
    case TOKEN_RETURN: token = RETURN; break;
    case TOKEN_VOID: token = VOID; break;
    case TOKEN_WHILE: token = WHILE; break;
    case TOKEN_LE: token = LE; break;
    case TOKEN_GE: token = GE; break;
    case TOKEN_EQ: token = EQ; break;
    case TOKEN_NE: token = NE; break;
    case TOKEN_PLUS: token = PLUS; break;
    case TOKEN_MINUS: token = MINUS; break;
    case TOKEN_TIMES: token = TIMES; break;
    case TOKEN_OVER: token = OVER; break;
    case TOKEN_LT: token = LT; break;
    case TOKEN_GT: token = GT; break;
    case TOKEN_ASSIGN: token = ASSIGN; break;
    case TOKEN_SEMI: token = SEMI; break;
    case TOKEN_COMMA: token = COMMA; break;
    case TOKEN_LPAREN: token = LPAREN; break;
    case TOKEN_RPAREN: token = RPAREN; break;
    case TOKEN_LBRACKET: token = LBRACKET; break;
    case TOKEN_RBRACKET: token = RBRACKET; break;
    case TOKEN_LBRACE: token = LBRACE; break;
    case TOKEN_RBRACE: token = RBRACE; break;
    case TOKEN_ID: token = ID; break;
    case TOKEN_NUM: token = NUM; break;
    default: return YYUNDEF;
    }

    yylval.node = ast_token(token_kind_name(kind), current_token.lexeme,
                            current_token.length, current_token.line,
                            current_token.column);
    return token;
}

static void yyerror(const char *message) {
    if (lexical_errors)
        return;
    syntax_errors++;
    fprintf(stderr, "Erro sintatico na linha %d, coluna %d: %s\n",
      yylloc.first_line, yylloc.first_column, message);
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    int close_input = 0;
    int parse_status;

    if (argc > 2) {
        fprintf(stderr, "Uso: %s [arquivo.cm]\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        input = fopen(argv[1], "rb");
        if (!input) {
            perror(argv[1]);
            return EXIT_FAILURE;
        }
        close_input = 1;
    }
    if (!lexer_init(input)) {
        fprintf(stderr, "Erro: nao foi possivel ler a entrada.\n");
        if (close_input)
            fclose(input);
        return EXIT_FAILURE;
    }
    if (close_input)
        fclose(input);

    lexer_rewind();
    {
        Token scan_token;
        TokenKind scan_kind;
        for (;;) {
            scan_kind = lexer_next_token(&scan_token);
            if (scan_kind == TOKEN_EOF)
                break;
            if (scan_kind == TOKEN_ERROR) {
                fprintf(stderr, "Erro lexico na linha %d, coluna %d: %s\n",
                        scan_token.line, scan_token.column,
                        scan_token.error_message);
                lexical_errors++;
            }
        }
    }
    lexer_rewind();

    if (lexical_errors == 0)
        parse_status = yyparse();
    else
        parse_status = 1;

    lexer_destroy();

    if (parse_status == 0 && !lexical_errors && !syntax_errors) {
        puts("Codigo sintaticamente valido.");
      puts("Arvore sintatica:");
      ast_print(stdout, parse_tree, 0);
      ast_free(parse_tree);
        return EXIT_SUCCESS;
    }
    ast_free(parse_tree);
    puts("Codigo invalido.");
    return EXIT_FAILURE;
}
