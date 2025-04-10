
#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <cassert>
#include <cstring>

namespace mlang {

class type_t;

class context {
public:
    void set_global_variable(std::string_view name, const type_t* value);

    void set_local_variable(std::string_view name, const type_t* value);

    std::optional<const type_t *> get_variable(std::string_view name);

    inline void new_scope();

private:
    std::list<std::unordered_map<std::string, const type_t *>> variables_;
};

class statement {
public:
    virtual void execute(context *ctx) const = 0;
};

class expression : public statement {
public:
    virtual const mlang::type_t *value() const = 0;
    virtual void execute(context *ctx) const { value(); }
};

enum class type_kind {
    INTEGER,
    STRING,
    STRUCT
};

class type_t : public expression {
public:
    virtual type_kind get_kind() const = 0;
};

class integer_type : public type_t {
public:

    integer_type(uint32_t data) : data_(data) {}
    integer_type(uint8_t *data, size_t len) {
        assert(len == 4);
        memcpy(&data_, data, len);
    }

    type_kind get_kind() const { return type_kind::INTEGER; };

    const type_t *value() const { return this; }

    integer_type operator+(integer_type const& obj) { return integer_type(data_ + obj.data_); }
    integer_type operator-(integer_type const& obj) { return integer_type(data_ - obj.data_); }
    integer_type operator*(integer_type const& obj) { return integer_type(data_ * obj.data_); }
    integer_type operator/(integer_type const& obj) { return integer_type(data_ / obj.data_); }
private:
    uint32_t data_;
};

class expr_list : public expression {
public:
    expr_list(std::vector<expression *> *exprs) : exprs_(exprs) {}

    const type_t *value() const { return exprs_->back()->value(); }

private:
    const std::vector<expression *> *exprs_;
};

class var_decl : public expression {
public:
    var_decl(std::string_view name, expression *expr) : var_name_(name), expr_(expr) {}
    
    virtual void execute(context *ctx) const { ctx->set_local_variable(var_name_, 
                                                                        value()  ); }

    const type_t *value() const { return expr_->value(); }

private:
    const std::string var_name_;
    const expression *expr_;
};

}
