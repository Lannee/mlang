
#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <cassert>
#include <cstring>
#include <iostream>

namespace mlang {

class type;
class unit_type;

class context {
public:

    context() : variables_(1) {}

    void set_global_variable(std::string_view name, const type* value);

    void set_local_variable(std::string_view name, const type* value);

    const type *get_variable(std::string_view name);

    inline void new_scope();

    void print_state(std::ostream &os) const;

    // ~context();

private:
    std::list<std::unordered_map<std::string, const type *>> variables_;
};

class expression {
public:
    virtual const mlang::type *value(context &ctx) const = 0;
    virtual ~expression() {}
};

enum class type_kind {
    UNIT,
    INTEGER,
    STRING,
    STRUCT
};

class type : public expression {
public:
    virtual type_kind get_kind() const = 0;
    virtual std::string repr() const = 0;
};

class unit_type : public type {
public:
    type_kind get_kind() const { return type_kind::UNIT; };
    const type *value(context &_) const { return this; }
    std::string repr() const { return "T"; }  
};

const unit_type UNIT__{};

class statement : public expression {
public:
    const mlang::type *value(context &ctx) const { execute(ctx); return &UNIT__; }
    virtual void execute(context &ctx) const = 0;
};

class print_statement : public statement {
public:
    print_statement(const std::vector<const mlang::expression *> *exprs) : exprs_(exprs) {}
    void execute(context &ctx) const;
    ~print_statement();

private:
    const std::vector<const mlang::expression *> *exprs_;
};

class integer_type : public type {
public:

    integer_type(uint32_t data) : data_(data) {}

    type_kind get_kind() const { return type_kind::INTEGER; };

    const type *value(context &_) const { return this; }

    std::string repr() const { return std::to_string(data_); }  

    integer_type operator+(integer_type const& obj) { return integer_type(data_ + obj.data_); }
    integer_type operator-(integer_type const& obj) { return integer_type(data_ - obj.data_); }
    integer_type operator*(integer_type const& obj) { return integer_type(data_ * obj.data_); }
    integer_type operator/(integer_type const& obj) { return integer_type(data_ / obj.data_); }
private:
    int data_;
};

class string_type : public type {
public:

    string_type(std::string_view data) : data_(data) {}

    type_kind get_kind() const { return type_kind::STRING; };

    const type *value(context &_) const { return this; }

    std::string repr() const { return data_; }  

    string_type operator+(string_type const& obj) { return string_type(data_ + obj.data_); }
private:
    std::string data_;
};

class expr_list : public expression {
public:
    expr_list(std::vector<const expression *> *exprs) : exprs_(exprs) {}

    const type *value(context &ctx) const { return exprs_->back()->value(ctx); }

private:
    const std::vector<const expression *> *exprs_;
};

class var_decl : public expression {
public:
    var_decl(std::string_view name, const expression *expr) : var_name_(name), expr_(expr) { }

    const type *value(context &ctx) const { auto value = expr_->value(ctx);
                                            ctx.set_local_variable(var_name_, value );
                                            return value; }

    ~var_decl();                                            

private:
    const std::string var_name_;
    const expression *expr_;
};

class variable : public expression {
public:
    variable(std::string_view name) : name_(name) {}
    const type *value(context &ctx) const { return ctx.get_variable(name_)->value(ctx); }

private:
    const std::string name_;
};

}
