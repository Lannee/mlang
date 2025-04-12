
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
inline void context::pop_scope() { variables_.pop_front();     }

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

const type *if_expression::value(context &ctx) const {
    if (cond_->value(ctx)->to_integer_type().data__() > 0)
        return then_->value(ctx);
    if (otherwise_) return otherwise_->value(ctx);
    return &UNIT__;
}

if_expression::~if_expression() {
    delete cond_;
    delete then_;
    delete otherwise_;
}

void until_statement::execute(context &ctx) const {
    while (cond_->value(ctx)->to_integer_type().data__() > 0)
        body_->value(ctx);
}

until_statement::~until_statement() {
    delete cond_;
    delete body_;
}

var_decl::~var_decl() { delete expr_; }

const type *expr_list::value(context &ctx) const {
    // Creating local scope
    ctx.new_scope();

    const type *last_expr_value = &UNIT__;
    for(auto e : *exprs_)
        last_expr_value = e->value(ctx);
    
    ctx.pop_scope();

    return last_expr_value; 
}

}