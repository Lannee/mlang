
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
class integer_type;

class context {
public:

    context() : variables_(0) {}

    void set_global_variable(std::string_view name, const type* value);

    void set_local_variable(std::string_view name, const type* value);

    const type *get_variable(std::string_view name);

    inline void new_scope();
    inline void pop_scope();

    void print_state() const;

    // ~context();

private:
    std::list<std::unordered_map<std::string, const type *>> variables_;
};

class expression {
public:
    virtual const type *value(context &ctx) const = 0;
    virtual ~expression() {}
};

enum type_kind {
    UNIT,
    INTEGER,
    STRING,
    STRUCT
};

class type : public expression {
public:
    virtual type_kind kind() const = 0;
    virtual std::string repr() const = 0;
    virtual integer_type to_integer_type() const = 0;
};

class integer_type : public type {
public:

    integer_type(uint32_t data) : data_(data) {}

    type_kind kind() const { return type_kind::INTEGER; };

    const type *value(context &_) const { return this; }

    std::string repr() const { return std::to_string(data_); }  
    integer_type to_integer_type() const override { return *this; }

    uint32_t data__() const { return data_; }

    integer_type operator+ (integer_type const& obj) const { return data_ +  obj.data_; }
    integer_type operator- (integer_type const& obj) const { return data_ -  obj.data_; }
    integer_type operator* (integer_type const& obj) const { return data_ *  obj.data_; }
    integer_type operator/ (integer_type const& obj) const { return data_ /  obj.data_; }
    integer_type operator> (integer_type const& obj) const { return data_ >  obj.data_; }
    integer_type operator< (integer_type const& obj) const { return data_ <  obj.data_; }
    integer_type operator>=(integer_type const& obj) const { return data_ >= obj.data_; }
    integer_type operator<=(integer_type const& obj) const { return data_ <= obj.data_; }
    integer_type operator==(integer_type const& obj) const { return data_ == obj.data_; }
    integer_type operator!=(integer_type const& obj) const { return data_ != obj.data_; }
private:
    uint32_t data_;
};

class unit_type : public type {
public:
    type_kind kind() const { return type_kind::UNIT; };
    const type *value(context &_) const { return this; }
    std::string repr() const { return "T"; } 
    integer_type to_integer_type() const override { return 0; } 
};

const unit_type UNIT__{};

class statement : public expression {
public:
    const type *value(context &ctx) const { execute(ctx); return &UNIT__; }
    virtual void execute(context &ctx) const = 0;
};

class print_function : public expression {
public:
    print_function(const std::vector<const expression *> *exprs) : exprs_(exprs) {}
    const type *value(context &ctx) const;
    ~print_function();

private:
    const std::vector<const expression *> *exprs_;
};

class until_statement : public statement {
public:
    until_statement(const expression *cond, const expression *body) : cond_(cond), body_(body) {}
    void execute(context &ctx) const;
    ~until_statement();

private:
    const expression *cond_;
    const expression *body_;
};


class if_expression : public expression {
public:
    if_expression(const expression *cond, const expression *then, const expression *otherwise) : cond_(cond), then_(then), otherwise_(otherwise) {}
    const type *value(context &ctx) const;
    ~if_expression();

private:
    const expression *cond_;
    const expression *then_;
    const expression *otherwise_;
};

class string_type : public type {
public:

    string_type(std::string_view data) : data_(data) {}

    type_kind kind() const { return type_kind::STRING; };

    const type *value(context &_) const { return this; }

    std::string repr() const { return data_; } 
    integer_type to_integer_type() const override { return data_ != ""; }  

    string_type operator+(string_type const& obj) { return string_type(data_ + obj.data_); }
private:
    std::string data_;
};

class expr_list : public expression {
public:
    expr_list(std::vector<const expression *> *exprs) : exprs_(exprs) {}

    const type *value(context &ctx) const;

private:
    const std::vector<const expression *> *exprs_;
};

class var_decl : public expression {
public:
    var_decl(std::string_view name, const expression *expr) : var_name_(name), expr_(expr) { }

    const type *value(context &ctx) const;

    ~var_decl();                                            

private:
    const std::string var_name_;
    const expression *expr_;
};

class variable : public expression {
public:
    variable(std::string_view name) : name_(name) {}
    const type *value(context &ctx) const;

private:
    const std::string name_;
};


enum comp_kind {
    EQUAL,
    NOTEQUAL,
    GREATER,
    GREATEREQUAL,
    LESS,
    LESSEQUAL
};

class comp_expression : public expression {
public:
    comp_expression(const expression *l, comp_kind op, const expression *r) : l_(l), op_(op), r_(r) {}
    const type *value(context &ctx) const;
private:
    const expression *r_;
    comp_kind op_;
    const expression *l_;
};

class function_call : public expression {
public:
    function_call(std::string_view name, const std::vector<const expression *> *args) : name_(name), args_(args) {}
    const type *value(context &ctx) const;
    ~function_call();

private:
    const std::string name_; 
    const std::vector<const expression *> *args_;
};


class tostr_function : public expression {
public:
    tostr_function(const expression *arg) : arg_(arg) {}
    const string_type *value(context &ctx) const;
    ~tostr_function() { delete arg_; }

private:
    const expression *arg_;
};

class toint_function : public expression {
public:
    toint_function(const expression *arg) : arg_(arg) {}
    const integer_type *value(context &ctx) const;
    ~toint_function() { delete arg_; }

private:
    const expression *arg_;
};

}
