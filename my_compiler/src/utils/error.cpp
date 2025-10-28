#include <iostream>
#include <string>

namespace cmini {

void reportSyntaxError(const std::string& file, int line, int col, const std::string& msg) {
    std::cerr << file << ":" << line << ":" << col << ": error: " << msg << "\n";
}

} // namespace cmini
