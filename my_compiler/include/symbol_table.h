#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "type_system.h"

namespace cmini {

struct Symbol {
    Type type;
    bool isFunction {false};
    std::vector<Type> paramTypes; // for functions
};

struct Scope {
    std::unordered_map<std::string, Symbol> table;
    Scope* parent {nullptr};
    explicit Scope(Scope* p=nullptr): parent(p) {}
    Symbol* lookupLocal(const std::string& n);
    Symbol* lookup(const std::string& n);
    void insert(const std::string& n, const Symbol& s);
};

} // namespace cmini
