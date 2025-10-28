#include "type_system.h"
#include <sstream>
#include <unordered_map>

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

static std::unordered_map<std::string, std::vector<StructMember>>& structRegistry() {
    static std::unordered_map<std::string, std::vector<StructMember>> reg;
    return reg;
}

void registerStruct(const std::string& tag, const std::vector<StructMember>& members) {
    structRegistry()[tag] = members;
}

bool isStructDefined(const std::string& tag) {
    return structRegistry().find(tag) != structRegistry().end();
}

const std::vector<StructMember>* getStructMembers(const std::string& tag) {
    auto it = structRegistry().find(tag);
    if (it == structRegistry().end()) return nullptr;
    return &it->second;
}

} // namespace cmini
