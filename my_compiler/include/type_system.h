#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace cmini {

enum class BaseType { Void, Int, Char, Float };

enum class NamedKind { None, Struct, Enum, Union };

struct Type {
    BaseType base {BaseType::Int};
    int pointerLevels {0};
    std::vector<size_t> arrayDims; // multi-dimensional array sizes outermost-first
    NamedKind namedKind {NamedKind::None};
    std::string namedTag; // enum/union tag name if any

    static Type voidTy();
    static Type intTy();

    std::string toString() const;
};

// Minimal struct registry to record tags and member lists
struct StructMember {
    std::string name;
    Type type;
};

// Simple global registry API (implemented in type_system.cpp)
void registerStruct(const std::string& tag, const std::vector<StructMember>& members);
bool isStructDefined(const std::string& tag);
const std::vector<StructMember>* getStructMembers(const std::string& tag);

} // namespace cmini
