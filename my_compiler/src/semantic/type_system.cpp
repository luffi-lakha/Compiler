#include "type_system.h"
#include <sstream>

namespace cmini {

Type Type::voidTy() { Type t; t.base = BaseType::Void; return t; }
Type Type::intTy() { Type t; t.base = BaseType::Int; return t; }

std::string Type::toString() const {
    std::ostringstream os;
    switch (base) {
        case BaseType::Void: os << "void"; break;
        case BaseType::Int: os << "int"; break;
        case BaseType::Char: os << "char"; break;
        case BaseType::Float: os << "float"; break;
    }
    for (int i=0;i<pointerLevels;++i) os << "*";
    for (size_t n : arrayDims) os << "[" << n << "]";
    return os.str();
}

} // namespace cmini
