#pragma once

#include "expression.hh"
#include <cstdint>
#include <cassert>
#include <cstring>

namespace mlang {

enum class type_kind {
    INTEGER,
    STRING,
    STRUCT
};


class type : public expression {
public:
    virtual type_kind get_kind() const = 0;

};

class interger_type : public type {
public:

    interger_type(uint32_t data) : data_(data) {}
    interger_type(uint8_t *data, size_t len) {
        assert(len == 4);
        memcpy(&data_, data, len);
    }

    type_kind get_kind() const { return type_kind::INTEGER; };

    const type *value() const { return this; }

    interger_type operator+(interger_type const& obj) { return interger_type(data_ + obj.data_); }
    interger_type operator-(interger_type const& obj) { return interger_type(data_ - obj.data_); }
    interger_type operator*(interger_type const& obj) { return interger_type(data_ * obj.data_); }
    interger_type operator/(interger_type const& obj) { return interger_type(data_ / obj.data_); }
private:
    uint32_t data_;
};

}