
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

void context::print_state() const {
    for(auto scope : variables_) {
        std::cout << "scope" << std::endl;
        for (auto &[n, t] : scope) {
            std::cout << "name: " << n << " value: " << (t ? t->repr() : "null") << std::endl;
        }
    }
}

// context::~context() {
//     for(auto &m : variables_)
//         for(auto [_, t] : m) delete t;
// }

void print_statement::execute(context &ctx) const {
    for(const auto &expr : *exprs_)
        std::cout << expr->value(ctx)->repr(); 
}

print_statement::~print_statement() {
    for(auto *e : *exprs_) delete e;
    delete exprs_;
}

var_decl::~var_decl() { delete expr_; }

const type *expr_list::value(context &ctx) const {
    const type *last_expr_value = &UNIT__;
    for(auto e : *exprs_) {
        last_expr_value = e->value(ctx);
    }
    return last_expr_value; 
}

}