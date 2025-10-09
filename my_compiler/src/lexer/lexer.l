%option noyywrap
%{
#include "parser.tab.h"
#include <cstdio>
#include <cstdlib>
int yylineno = 1;
%}

WS      [ \t\r]+
ID      [a-zA-Z_][a-zA-Z0-9_]*
NUM     [0-9]+

%%

{WS}                ;
"//".*              { yylineno++; }
"/*"([^*]|\*+[^*/])*"*/" {
  const char* s = yytext; for (; *s; ++s) if (*s=='\n') ++yylineno;
}
\n                  { ++yylineno; }

"int"              { return KW_INT; }
"char"             { return KW_CHAR; }
"float"            { return KW_FLOAT; }
"void"             { return KW_VOID; }
"if"               { return KW_IF; }
"else"             { return KW_ELSE; }
"for"              { return KW_FOR; }
"while"            { return KW_WHILE; }
"do"               { return KW_DO; }
"return"           { return KW_RETURN; }
"break"            { return KW_BREAK; }
"continue"         { return KW_CONTINUE; }

"&&"               { return AND_AND; }
"||"               { return OR_OR; }
"=="               { return EQ; }
"!="               { return NE; }
"<="               { return LE; }
">="               { return GE; }
"<<"               { return SHL; }
">>"               { return SHR; }

"="                { return '='; }
"+"                { return '+'; }
"-"                { return '-'; }
"*"                { return '*'; }
"/"                { return '/'; }
"%"                { return '%'; }
"&"                { return '&'; }
"|"                { return '|'; }
"^"                { return '^'; }
"~"                { return '~'; }
"<"                { return LT; }
">"                { return GT; }
"("                { return '('; }
")"                { return ')'; }
"{"                { return '{'; }
"}"                { return '}'; }
"["                { return '['; }
"]"                { return ']'; }
","                { return ','; }
";"                { return ';'; }

{NUM}               { return INTEGER; }
{ID}                { return IDENTIFIER; }

.                   { /* ignore unknown single char */ }

%%
