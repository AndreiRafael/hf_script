#ifndef HFS_SUBCALLOPERATION_HPP
#define HFS_SUBCALLOPERATION_HPP

#include "HFS_SequentialOperation.hpp"

namespace hfs {
    class SubCallOperation : public SequentialOperation {
    protected:
        OperationResult internal_run(Scope* const scope,
                            const std::vector<Variable>& values,
                            Variable* const returned_value,
                            Operation** const next_operation,
                            Scope** const next_scope) const final;
    public:
        SubCallOperation(Operation* operation);
    };
}

#endif//HFS_SUBCALLOPERATION_HPP