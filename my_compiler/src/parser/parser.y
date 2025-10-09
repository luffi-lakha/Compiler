%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
int yylex(void);
void yyerror(const char* s);
extern int yylineno;
%}

%define parse.error verbose

%token KW_INT KW_CHAR KW_FLOAT KW_VOID KW_IF KW_ELSE KW_FOR KW_WHILE KW_DO KW_RETURN KW_BREAK KW_CONTINUE
%token IDENTIFIER INTEGER
%token OR_OR AND_AND EQ NE LE GE SHL SHR LT GT

%left OR_OR
%left AND_AND
%left '|' '^' '&'
%left EQ NE
%left LT GT LE GE
%left SHL SHR
%left '+' '-'
%left '*' '/' '%'
%right UPLUS UMINUS

%%
program
  : external_list
  ;

external_list
  : /* empty */
  | external_list external
  ;

external
  : function
  | decl ';'
  ;

function
  : type IDENTIFIER '(' opt_params ')' compound
  ;

type
  : KW_INT
  | KW_CHAR
  | KW_FLOAT
  | KW_VOID
  ;

opt_params
  : /* empty */
  | param_list
  ;

param_list
  : param
  | param_list ',' param
  ;

param
  : type IDENTIFIER
  ;

decl
  : type declarator
  | type declarator '=' expr
  ;

declarator
  : IDENTIFIER
  | declarator '[' INTEGER ']'
  | '*' declarator
  ;

compound
  : '{' stmt_list_opt '}'
  ;

stmt_list_opt
  : /* empty */
  | stmt_list_opt stmt
  ;

stmt
  : decl ';'
  | expr ';'
  | KW_RETURN expr ';'
  | KW_RETURN ';'
  | compound
  | KW_IF '(' expr ')' stmt
  | KW_IF '(' expr ')' stmt KW_ELSE stmt
  | KW_WHILE '(' expr ')' stmt
  | KW_DO stmt KW_WHILE '(' expr ')' ';'
  | KW_FOR '(' opt_expr ';' opt_expr ';' opt_expr ')' stmt
  | KW_BREAK ';'
  | KW_CONTINUE ';'
  ;

opt_expr
  : /* empty */
  | expr
  ;

expr
  : assign
  ;

assign
  : logic_or
  | unary '=' assign
  ;

logic_or
  : logic_and
  | logic_or OR_OR logic_and
  ;

logic_and
  : bit_or
  | logic_and AND_AND bit_or
  ;

bit_or
  : bit_xor
  | bit_or '|' bit_xor
  ;

bit_xor
  : bit_and
  | bit_xor '^' bit_and
  ;

bit_and
  : equality
  | bit_and '&' equality
  ;

equality
  : relational
  | equality EQ relational
  | equality NE relational
  ;

relational
  : shift
  | relational LT shift
  | relational GT shift
  | relational LE shift
  | relational GE shift
  ;

shift
  : additive
  | shift SHL additive
  | shift SHR additive
  ;

additive
  : multiplicative
  | additive '+' multiplicative
  | additive '-' multiplicative
  ;

multiplicative
  : unary
  | multiplicative '*' unary
  | multiplicative '/' unary
  | multiplicative '%' unary
  ;

unary
  : primary
  | '+' unary %prec UPLUS
  | '-' unary %prec UMINUS
  | '*' unary
  | '&' unary
  ;

primary
  : IDENTIFIER
  | INTEGER
  | '(' expr ')'
  ;

%%

void yyerror(const char* s) {
  std::fprintf(stderr, "parser error at line %d: %s\n", yylineno, s);
}
