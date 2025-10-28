#include "symbol_table.h"

namespace cmini {

Symbol* Scope::lookupLocal(const std::string& n) {
    auto it = table.find(n);
    if (it == table.end()) return nullptr;
    return &it->second;
}

Symbol* Scope::lookup(const std::string& n) {
    for (Scope* s = this; s != nullptr; s = s->parent) {
        auto it = s->table.find(n);
        if (it != s->table.end()) return &it->second;
    }
    return nullptr;
}

void Scope::insert(const std::string& n, const Symbol& s) {
    table[n] = s;
}

} // namespace cmini
