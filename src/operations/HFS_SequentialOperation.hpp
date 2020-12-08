#ifndef HFS_SEQUENTIALOPERATION_HPP
#define HFS_SEQUENTIALOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class SequentialOperation : public Operation {
    protected:
        Operation* next_operation = nullptr;
    public:
        void set_next_operation(Operation* operation);
    };
}

#endif