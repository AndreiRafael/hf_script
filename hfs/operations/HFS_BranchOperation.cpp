#include "HFS_BranchOperation.hpp"

#include <algorithm>

namespace hfs {
    BranchOperation::BranchOperation(Operation* const value) {
        add_requirement(value);
    }

    OperationResult BranchOperation::internal_run(ScriptHolder* holder,
                                                  Scope* const scope,
                                                  const std::vector<Variable>& values,
                                                  Variable* const return_value,
                                                  Operation** const next_operation,
                                                  Scope** const next_scope) const
    {
        *next_operation = operation_array[values[0].get_boolean_value()];

        *next_scope = scope;
        *return_value = Variable::create_null();
        return OperationResult::Return;
    }

    void BranchOperation::set_true_operation(Operation* const operation) {
        operation_array[0] = operation;
    }

    void BranchOperation::set_false_operation(Operation* const operation) {
        operation_array[1] = operation;
    }
}