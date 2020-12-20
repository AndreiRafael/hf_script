#ifndef HFS_ESSENTIALS_HPP
#define HFS_ESSENTIALS_HPP

#include "../HFS_Variable.hpp"
#include <vector>

namespace hfs {
    Variable equal(std::vector<Variable> values);
    Variable not(std::vector<Variable> values);
    Variable and(std::vector<Variable> values);
    Variable or(std::vector<Variable> values);

    Variable less(std::vector<Variable> values);
    Variable greater(std::vector<Variable> values);
}

#endif