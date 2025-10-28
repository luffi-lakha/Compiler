%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include "ast.h"
#include "type_system.h"

int yylex(void);
void yyerror(const char* s);
extern int yylineno;

static cmini::Program* g_program = nullptr;
extern "C" cmini::Program* g_program_extern_ref() { return g_program; }

struct DeclAgg {
  std::string name;
  int pointerLevels = 0;
  std::vector<size_t> arrayDims;
};

static std::vector<cmini::StructMember> g_struct_members;
%}

%code requires {
  #include <string>
  #include <vector>
  #include "ast.h"
  #include "type_system.h"
  struct DeclAgg;
}

%define parse.error verbose

%union {
  long ival;
  char* str;
  cmini::Type* type;
  cmini::Expr* expr;
  cmini::Stmt* stmt;
  cmini::Block* block;
  cmini::Function* func;
  DeclAgg* declagg;
  std::vector<cmini::Param>* params;
  cmini::Param* param;
}

%token KW_INT KW_CHAR KW_FLOAT KW_VOID KW_STRUCT KW_RETURN
%token <str> IDENTIFIER
%token <ival> INTEGER

%left '+' '-'
%left '*' '/'

%type <type> type base_type
%type <stmt> stmt decl
%type <block> compound opt_stmt_list
%type <expr> expr assign additive mul unary primary
%type <declagg> declarator
%type <param> param
%type <params> opt_params param_list

%%

program
  : items               { if (!g_program) g_program = new cmini::Program(); }
  ;

items
  : /* empty */
  | items item
  ;

item
  : function
  | struct_def
  ;

struct_def
  : KW_STRUCT IDENTIFIER '{' struct_members '}' ';'
    {
      cmini::registerStruct(std::string($2), g_struct_members);
      free($2);
      g_struct_members.clear();
    }
  | KW_STRUCT '{' struct_members '}' IDENTIFIER ';'
    {
      cmini::registerStruct(std::string($5), g_struct_members);
      free($5);
      g_struct_members.clear();
    }
  ;

struct_members
  : struct_member
  | struct_members struct_member
  ;

struct_member
  : type IDENTIFIER ';'
    {
      cmini::StructMember m; m.name = std::string($2); m.type = *$1;
      g_struct_members.push_back(std::move(m));
      delete $1; free($2);
    }
  ;

function
  : type IDENTIFIER '(' opt_params ')' compound
    {
      if (!g_program) g_program = new cmini::Program();
      cmini::Function* f = new cmini::Function();
      f->retType = *$1;
      f->name = std::string($2);
      if ($4) { f->params = *$4; delete $4; }
      f->body = std::unique_ptr<cmini::Block>($6);
      delete $1; free($2);
      g_program->functions.emplace_back(f);
    }
  ;

opt_params
  : /* empty */          { $$ = nullptr; }
  | param_list           { $$ = $1; }
  ;

param_list
  : param                { $$ = new std::vector<cmini::Param>{ *$1 }; delete $1; }
  | param_list ',' param { $1->push_back(*$3); delete $3; $$ = $1; }
  ;

param
  : type IDENTIFIER      { $$ = new cmini::Param{ *$1, std::string($2) }; delete $1; free($2); }
  ;

base_type
  : KW_INT               { $$ = new cmini::Type(cmini::Type::intTy()); }
  | KW_CHAR              { cmini::Type t; t.base=cmini::BaseType::Char; $$ = new cmini::Type(t); }
  | KW_FLOAT             { cmini::Type t; t.base=cmini::BaseType::Float; $$ = new cmini::Type(t); }
  | KW_VOID              { $$ = new cmini::Type(cmini::Type::voidTy()); }
  | KW_STRUCT IDENTIFIER { cmini::Type t; t.base=cmini::BaseType::Int; t.namedKind=cmini::NamedKind::Struct; t.namedTag=std::string($2); $$=new cmini::Type(t); free($2); }
  ;

type
  : base_type            { $$ = $1; }
  ;

stmt
  : decl ';'             { $$ = $1; }
  | expr ';'             { $$ = new cmini::ExprStmt(std::unique_ptr<cmini::Expr>($1)); }
  | KW_RETURN expr ';'   { auto r=new cmini::ReturnStmt(); r->expr=std::unique_ptr<cmini::Expr>($2); $$=r; }
  | KW_RETURN ';'        { $$ = new cmini::ReturnStmt(); }
  | compound             { $$ = $1; }
  ;

decl
  : type declarator
    {
      cmini::Type t = *$1; t.pointerLevels += $2->pointerLevels; t.arrayDims = $2->arrayDims;
      auto d = new cmini::Decl(t, $2->name);
      $$ = d;
      delete $1; delete $2;
    }
  | type declarator '=' expr
    {
      cmini::Type t = *$1; t.pointerLevels += $2->pointerLevels; t.arrayDims = $2->arrayDims;
      auto d = new cmini::Decl(t, $2->name);
      d->init = std::unique_ptr<cmini::Expr>($4);
      $$ = d;
      delete $1; delete $2;
    }
  ;

declarator
  : IDENTIFIER           { auto d = new DeclAgg(); d->name = $1; free($1); $$ = d; }
  | '*' declarator       { $2->pointerLevels++; $$ = $2; }
  | declarator '[' INTEGER ']' { $1->arrayDims.push_back((size_t)$3); $$ = $1; }
  ;

compound
  : '{' opt_stmt_list '}' { $$ = $2 ? $2 : new cmini::Block(); }
  ;

opt_stmt_list
  : /* empty */          { $$ = nullptr; }
  | opt_stmt_list stmt   { cmini::Block* b = $1 ? $1 : new cmini::Block(); b->items.emplace_back($2); $$ = b; }
  ;

expr
  : assign               { $$ = $1; }
  ;

assign
  : additive             { $$ = $1; }
  | primary '=' assign   { $$ = new cmini::AssignExpr(std::unique_ptr<cmini::Expr>($1), std::unique_ptr<cmini::Expr>($3)); }
  ;

additive
  : mul                  { $$ = $1; }
  | additive '+' mul     { $$ = new cmini::BinaryExpr(cmini::BinaryOp::Add, std::unique_ptr<cmini::Expr>($1), std::unique_ptr<cmini::Expr>($3)); }
  | additive '-' mul     { $$ = new cmini::BinaryExpr(cmini::BinaryOp::Sub, std::unique_ptr<cmini::Expr>($1), std::unique_ptr<cmini::Expr>($3)); }
  ;

mul
  : unary                { $$ = $1; }
  | mul '*' unary        { $$ = new cmini::BinaryExpr(cmini::BinaryOp::Mul, std::unique_ptr<cmini::Expr>($1), std::unique_ptr<cmini::Expr>($3)); }
  | mul '/' unary        { $$ = new cmini::BinaryExpr(cmini::BinaryOp::Div, std::unique_ptr<cmini::Expr>($1), std::unique_ptr<cmini::Expr>($3)); }
  ;

unary
  : primary              { $$ = $1; }
  | '+' unary            { $$ = new cmini::UnaryExpr(cmini::UnaryOp::Plus, std::unique_ptr<cmini::Expr>($2)); }
  | '-' unary            { $$ = new cmini::UnaryExpr(cmini::UnaryOp::Minus, std::unique_ptr<cmini::Expr>($2)); }
  ;

primary
  : IDENTIFIER           { $$ = new cmini::VarRef(std::string($1)); free($1); }
  | INTEGER              { $$ = new cmini::IntegerLiteral($1); }
  | '(' expr ')'         { $$ = $2; }
  ;

%%

void yyerror(const char* s) {
  std::fprintf(stderr, "parser error at line %d: %s\n", yylineno, s);
}
