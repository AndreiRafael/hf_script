#ifndef HFS_WAITOPERATION_HPP
#define HFS_WAITOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class WaitOperation : public Operation {
    public:
        WaitOperation(const std::string variable_name, Operation* const value);

        OperationResult run(Scope* const scope,
                            std::vector<Variable> values,
                            Variable* const return_value,
                            Operation** const next_operation,
                            Scope** next_scope) const final;
    };
}

#endif//HFS_WAITOPERATION_HPP