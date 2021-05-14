#ifndef HFS_OPERATOR_HPP
#define HFS_OPERATOR_HPP

#include <string>

namespace hfs::core {
    enum class OperatorType {
        None   = 0b00,
        Single = 0b01,
        Double = 0b10,
        Any    = 0b11
    };

    struct Operator {
        std::string symbol;
        std::string function;
        OperatorType type;
    };
}

#endif