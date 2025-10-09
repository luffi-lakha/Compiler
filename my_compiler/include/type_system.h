#pragma once
#include <string>
#include <vector>

namespace cmini {

enum class BaseType { Void, Int, Char, Float };

enum class NamedKind { None, Enum, Union };

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

} // namespace cmini
