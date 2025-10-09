#include <string>
#include <fstream>
#include <sstream>
extern "C" {
  void yy_scan_string(const char*);
}

void scan_file_into_lexer(const std::string& path) {
  std::ifstream in(path);
  std::ostringstream ss; ss << in.rdbuf();
  yy_scan_string(ss.str().c_str());
}
