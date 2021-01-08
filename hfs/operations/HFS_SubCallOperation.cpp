#include "HFS_SubCallOperation.hpp"

namespace hfs {
    SubCallOperation::SubCallOperation(Operation* operation) {
        add_requirement(operation);
    }

    OperationResult SubCallOperation::internal_run(ScriptHolder* holder,
                                                   Scope* const scope,
                                                   const std::vector<Variable>& values,
                                                   Variable* const returned_value,
                                                   Operation** const next_operation,
                                                   Scope** const next_scope) const {
        *next_scope = scope;
        *next_operation = this->next_operation;
        *returned_value = values[0];
        return OperationResult::Return;
    }
}