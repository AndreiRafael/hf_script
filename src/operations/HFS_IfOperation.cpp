#include "HFS_IfOperation.hpp"

#include <algorithm>

namespace hfs {
    OperationResult IfOperation::run(Scope* const scope, std::vector<Variable> values, Variable* const return_value, Operation** const next_operation, Scope** next_scope) const {
        if(std::any_of(values.begin(), values.end(), [] (const Variable& var) { return var.get_type() == VariableType::Boolean && var.get_boolean_value(); } )) {
            *next_scope = new Scope(scope);// TODO: how do we get rid of this later?? maybe in the runner, if there's no next op, we just delete it
            *next_operation = true_operation;
            return OperationResult::Return;
        }

        *next_scope = scope;
        *next_operation = false_operation;
        return OperationResult::Return;
    }

    void IfOperation::set_true_operation(Operation* const operation) {
        true_operation = operation;
    }

    void IfOperation::set_false_operation(Operation* const operation) {
        false_operation = operation;
    }
}