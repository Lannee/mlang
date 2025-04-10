

#include "ast.hh"

namespace mlang {

void context::set_global_variable(std::string_view name, const type_t* value) {
    variables_.back()[std::string(name)] = value;
}

void context::set_local_variable(std::string_view name, const type_t* value) {
    variables_.front()[std::string(name)] = value;
}

std::optional<const type_t *> context::get_variable(std::string_view name) {
    auto _name = std::string(name);
    for (auto scope : variables_) {
        if(scope.count(std::string(_name))) return scope[_name];
    }

    return std::nullopt;
}

inline void context::new_scope() { variables_.emplace_front(); }

}