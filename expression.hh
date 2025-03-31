#pragma once

#include "types.hh"

#include <string>

namespace mlang {

class expression {
public:
    virtual const type *value() const = 0;
};

}
