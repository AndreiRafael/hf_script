#include "HFS_SequentialOperation.hpp"

namespace hfs {
    void SequentialOperation::set_next_operation(Operation* operation) {
        next_operation = operation;
    }
}