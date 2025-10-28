#pragma once
#include <string>
#include <iostream>

namespace cmini {

inline void logInfo(const std::string& msg) { std::cerr << "[info] " << msg << "\n"; }
inline void logError(const std::string& msg) { std::cerr << "[error] " << msg << "\n"; }

} // namespace cmini
