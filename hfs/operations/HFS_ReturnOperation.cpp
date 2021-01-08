#include "HFS_ReturnOperation.hpp"
#include "../HFS_ScriptHolder.hpp"

namespace hfs {
    ReturnOperation::ReturnOperation(Operation* operation) {
        add_requirement(operation);
    }

    OperationResult ReturnOperation::internal_run(ScriptHolder* holder,
                                                   Scope* const scope,
                                                   const std::vector<Variable>& values,
                                                   Variable* const returned_value,
                                                   Operation** const next_operation,
                                                   Scope** const next_scope) const
    {
        Scope* s = scope;
        while(s != holder->get_scope() && s != nullptr) {
            Scope* next = s->get_parent();
            delete s;
            s = next;
        }

        *next_scope = nullptr;
        *next_operation = nullptr;
        *returned_value = values[0];
        return OperationResult::Return;
    }
}