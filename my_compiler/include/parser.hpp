#pragma once
#include <memory>
#include <string>
#include "ast.h"

namespace cmini {

// Parse a source file using Flex/Bison and return an AST Program
std::unique_ptr<Program> parseFile(const std::string& path);

}
