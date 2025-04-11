
#include <ostream>

#include "ast.hh"

namespace mlang {

void context::set_global_variable(std::string_view name, const type* value) {
    variables_.back()[std::string(name)] = value;
}

void context::set_local_variable(std::string_view name, const type* value) {
    variables_.front()[std::string(name)] = value;
}

const type *context::get_variable(std::string_view name) {
    auto _name = std::string(name);
    for (auto scope : variables_) {
        if(scope.count(std::string(_name))) return scope[_name];
    }

    return nullptr;
}

inline void context::new_scope() { variables_.emplace_front(); }

}