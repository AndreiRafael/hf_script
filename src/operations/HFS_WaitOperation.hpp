#ifndef HFS_WAITOPERATION_HPP
#define HFS_WAITOPERATION_HPP

#include "HFS_SequentialOperation.hpp"

namespace hfs {
    class WaitOperation : public SequentialOperation {
    public:
        WaitOperation(const std::string variable_name, Operation* const value);

    protected:
        OperationResult internal_run(Scope* const scope,
                            const std::vector<Variable>& values,
                            Variable* const return_value,
                            Operation** const next_operation,
                            Scope** const next_scope) const final;
    };
}

#endif//HFS_WAITOPERATION_HPP