#include "lexer.h"
#include <cctype>
#include <optional>

namespace cmini {

static bool isIdentStart(char c) { return std::isalpha((unsigned char)c) || c=='_'; }
static bool isIdentCont(char c) { return std::isalnum((unsigned char)c) || c=='_'; }

Lexer::Lexer(const std::string& input) : src(input) {}

const Token& Lexer::peek() {
    if (!hasLookahead) { lookahead = scan(); hasLookahead = true; }
    return lookahead;
}

Token Lexer::next() {
    if (hasLookahead) { hasLookahead = false; return lookahead; }
    return scan();
}

bool Lexer::isAtEnd() const { return pos >= src.size(); }
char Lexer::current() const { return isAtEnd() ? '\0' : src[pos]; }
char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = src[pos++];
    if (c=='\n') { curLine++; curCol=1; } else { curCol++; }
    return c;
}
bool Lexer::match(char c) { if (current()==c) { advance(); return true; } return false; }

void Lexer::mark(Token& t, int startLine, int startCol) {
    t.line = startLine; t.col = startCol;
}

Token Lexer::scan() {
    // skip whitespace and comments
    while (!isAtEnd()) {
        char c = current();
        if (std::isspace((unsigned char)c)) { advance(); continue; }
        if (c=='/' && pos+1 < src.size() && src[pos+1]=='/') {
            advance(); advance(); while (!isAtEnd() && current()!='\n') advance(); continue;
        }
        if (c=='/' && pos+1 < src.size() && src[pos+1]=='*') {
            advance(); advance();
            while (!isAtEnd() && !(current()=='*' && pos+1<src.size() && src[pos+1]=='/')) advance();
            if (pos+1<src.size()) { advance(); advance(); }
            continue;
        }
        break;
    }
    if (isAtEnd()) return {TokenKind::End, "", 0, curLine, curCol};

    int startLine = curLine; int startCol = curCol;
    char c = advance();

    // identifiers and keywords
    if (isIdentStart(c)) {
        std::string s(1,c);
        while (isIdentCont(current())) s.push_back(advance());
        Token t; t.text=s; t.kind=TokenKind::Identifier; mark(t,startLine,startCol);
        if (s=="int") t.kind=TokenKind::KwInt;
        else if (s=="char") t.kind=TokenKind::KwChar;
        else if (s=="float") t.kind=TokenKind::KwFloat;
        else if (s=="void") t.kind=TokenKind::KwVoid;
        else if (s=="enum") t.kind=TokenKind::KwEnum;
        else if (s=="union") t.kind=TokenKind::KwUnion;
        else if (s=="if") t.kind=TokenKind::KwIf;
        else if (s=="else") t.kind=TokenKind::KwElse;
        else if (s=="for") t.kind=TokenKind::KwFor;
        else if (s=="while") t.kind=TokenKind::KwWhile;
        else if (s=="do") t.kind=TokenKind::KwDo;
        else if (s=="return") t.kind=TokenKind::KwReturn;
        else if (s=="break") t.kind=TokenKind::KwBreak;
        else if (s=="continue") t.kind=TokenKind::KwContinue;
        return t;
    }

    // numbers (decimal only for brevity)
    if (std::isdigit((unsigned char)c)) {
        long v = c - '0';
        while (std::isdigit((unsigned char)current())) v = v*10 + (advance()-'0');
        Token t; t.kind=TokenKind::Integer; t.intVal=v; mark(t,startLine,startCol); return t;
    }

    // strings and chars
    if (c=='"') {
        std::string s;
        while (!isAtEnd() && current()!='"') {
            char ch = advance();
            if (ch=='\\' && !isAtEnd()) {
                char n = advance();
                switch(n){case 'n': s+='\n'; break; case 't': s+='\t'; break; default: s+=n;}
            } else s.push_back(ch);
        }
        if (current()=='"') advance();
        Token t; t.kind=TokenKind::String; t.text=std::move(s); mark(t,startLine,startCol); return t;
    }
    if (c=='\'') {
        char v = advance();
        if (v=='\\') { char n = advance(); v = n=='n'?'\n':n; }
        if (current()=='\'') advance();
        Token t; t.kind=TokenKind::Char; t.intVal=v; mark(t,startLine,startCol); return t;
    }

    // punctuation and operators
    auto two = [&](char a,char b, TokenKind k)->std::optional<Token> {
        if (c==a && current()==b) { advance(); Token t; t.kind=k; t.text=std::string({a,b}); mark(t,startLine,startCol); return t; }
        return std::nullopt;
    };
    if (auto t=two('&','&',TokenKind::AndAnd)) return *t;
    if (auto t=two('|','|',TokenKind::OrOr)) return *t;
    if (auto t=two('=','=',TokenKind::EQ)) return *t;
    if (auto t=two('!','=',TokenKind::NE)) return *t;
    if (auto t=two('<','=',TokenKind::LE)) return *t;
    if (auto t=two('>','=',TokenKind::GE)) return *t;
    if (auto t=two('<','<',TokenKind::Shl)) return *t;
    if (auto t=two('>','>',TokenKind::Shr)) return *t;

    Token t; t.text = std::string(1,c); mark(t,startLine,startCol);
    switch (c) {
        case '+': t.kind=TokenKind::Plus; return t;
        case '-': t.kind=TokenKind::Minus; return t;
        case '*': t.kind=TokenKind::Star; return t;
        case '/': t.kind=TokenKind::Slash; return t;
        case '%': t.kind=TokenKind::Percent; return t;
        case '&': t.kind=TokenKind::Amp; return t;
        case '|': t.kind=TokenKind::Pipe; return t;
        case '^': t.kind=TokenKind::Caret; return t;
        case '~': t.kind=TokenKind::Tilde; return t;
        case '(': t.kind=TokenKind::LParen; return t;
        case ')': t.kind=TokenKind::RParen; return t;
        case '{': t.kind=TokenKind::LBrace; return t;
        case '}': t.kind=TokenKind::RBrace; return t;
        case '[': t.kind=TokenKind::LBracket; return t;
        case ']': t.kind=TokenKind::RBracket; return t;
        case ';': t.kind=TokenKind::Semicolon; return t;
        case ',': t.kind=TokenKind::Comma; return t;
        case '=': t.kind=TokenKind::Assign; return t;
        case '<': t.kind=TokenKind::LT; return t;
        case '>': t.kind=TokenKind::GT; return t;
        default: t.kind=TokenKind::End; return t;
    }
}

} // namespace cmini
