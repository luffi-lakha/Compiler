#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
int yyparse();
namespace cmini { void scan_file_into_lexer(const std::string& path); }
extern "C" cmini::Program* g_program_extern_ref();

namespace cmini {

// We'll switch to scanning from file content via yy_scan_string
std::unique_ptr<Program> parseFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open: " + path);
    std::ostringstream ss; ss << in.rdbuf();
    std::string text = ss.str();
    cmini::scan_file_into_lexer(path);
    if (yyparse() != 0) throw std::runtime_error("parse failed");
    // The grammar populates global g_program; if null, return empty
    Program* parsed = g_program_extern_ref();
    if (!parsed) return std::make_unique<Program>();
    std::unique_ptr<Program> result(parsed);
    return result;
}

} // namespace cmini
