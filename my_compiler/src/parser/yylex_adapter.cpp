#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <cstring>
#include "parser.tab.h"
#include "lexer.h"

// Expose yylineno for error messages
int yylineno = 1;

// Minimal bridge from our hand lexer to bison's yylex API
static std::unique_ptr<cmini::Lexer> g_lexer;

int yylex(void) {
  using namespace cmini;
  if (!g_lexer) return 0;
  Token t = g_lexer->next();
  yylineno = t.line;
  switch (t.kind) {
    case TokenKind::End: return 0;
    case TokenKind::Identifier:
      yylval.str = strdup(t.text.c_str());
      return IDENTIFIER;
    case TokenKind::Integer:
      yylval.ival = t.intVal;
      return INTEGER;
    case TokenKind::KwInt: return KW_INT;
    case TokenKind::KwChar: return KW_CHAR;
    case TokenKind::KwFloat: return KW_FLOAT;
    case TokenKind::KwVoid: return KW_VOID;
    case TokenKind::KwStruct: return KW_STRUCT;
    case TokenKind::KwReturn: return KW_RETURN;
    case TokenKind::Plus: return '+';
    case TokenKind::Minus: return '-';
    case TokenKind::Star: return '*';
    case TokenKind::Slash: return '/';
    case TokenKind::LParen: return '(';
    case TokenKind::RParen: return ')';
    case TokenKind::LBrace: return '{';
    case TokenKind::RBrace: return '}';
    case TokenKind::LBracket: return '[';
    case TokenKind::RBracket: return ']';
    case TokenKind::Semicolon: return ';';
    case TokenKind::Comma: return ',';
    case TokenKind::Assign: return '=';
    default:
      return 0;
  }
}

namespace cmini {

void scan_file_into_lexer(const std::string& path) {
  std::ifstream in(path);
  std::ostringstream ss; ss << in.rdbuf();
  g_lexer = std::make_unique<Lexer>(ss.str());
  yylineno = 1;
}

} // namespace cmini
