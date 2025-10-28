#include "ir_utils.h"

namespace cmini {

std::string typeToIR(const Type& t) {
    std::string b;
    switch (t.base) {
        case BaseType::Void: b = "void"; break;
        case BaseType::Int: b = "i32"; break;
        case BaseType::Char: b = "i8"; break;
        case BaseType::Float: b = "float"; break;
    }
    for (int i=0;i<t.pointerLevels;++i) b += "*";
    // arrays are lowered as pointers to first element for now
    if (!t.arrayDims.empty()) b += "*";
    return b;
}

} // namespace cmini
