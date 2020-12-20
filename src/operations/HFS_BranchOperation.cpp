#include "HFS_BranchOperation.hpp"

#include <algorithm>

namespace hfs {
    BranchOperation::BranchOperation(Operation* const value) {
        add_requirement(value);
    }

    OperationResult BranchOperation::internal_run(ScriptRunner* runner,
                                                  Scope* const scope,
                                                  const std::vector<Variable>& values,
                                                  Variable* const return_value,
                                                  Operation** const next_operation,
                                                  Scope** const next_scope) const
    {
        if(std::any_of(values.begin(),
                       values.end(),
                       [] (const Variable& var) { return var.get_type() == VariableType::Boolean && var.get_boolean_value(); } ))
        {
            *next_operation = true_operation;
        }
        else {
            *next_operation = false_operation;
        }

        *next_scope = scope;
        *return_value = Variable::create_null();
        return OperationResult::Return;
    }

    void BranchOperation::set_true_operation(Operation* const operation) {
        true_operation = operation;
    }

    void BranchOperation::set_false_operation(Operation* const operation) {
        false_operation = operation;
    }
}