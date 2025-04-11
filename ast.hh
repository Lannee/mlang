
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

    std::optional<const type *> get_variable(std::string_view name);

    inline void new_scope();

    void print_state(std::ostream &os) const;

private:
    std::list<std::unordered_map<std::string, const type *>> variables_;
};

class expression {
public:
    virtual const mlang::type *value(context &ctx) const = 0;
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

constexpr unit_type UNIT__{};

class statement : public expression {
public:
    const mlang::type *value(context &ctx) const { execute(ctx); return &UNIT__; }
    virtual void execute(context &ctx) const = 0;
};

class print_statement : public statement {
public:
    print_statement(const expression *expr) : expr_(expr) {}
    void execute(context &ctx) const { std::cout << expr_->value(ctx)->repr(); }

private:
    const expression *expr_;
};

class integer_type : public type {
public:

    integer_type(uint32_t data) : data_(data) {}
    integer_type(uint8_t *data, size_t len) {
        assert(len == 4);
        memcpy(&data_, data, len);
    }

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

private:
    const std::string var_name_;
    const expression *expr_;
};

}
