#include "HFS_FlattenScopeOperation.hpp"

namespace hfs {
    OperationResult FlattenScopeOperation::internal_run(ScriptRunner* runner,
                                                        Scope* const scope,
                                                        const std::vector<Variable>& values,
                                                        Variable* const returned_value,
                                                        Operation** const next_operation,
                                                        Scope** const next_scope) const
    {
        *returned_value = Variable::create_null();
        *next_operation = this->next_operation;

        auto parent_scope = scope->get_parent();
        if(parent_scope == nullptr) {
            *next_scope = nullptr;
            return OperationResult::Error;
        }

        delete scope;
        *next_scope = parent_scope;
        return OperationResult::Return;
    }
}