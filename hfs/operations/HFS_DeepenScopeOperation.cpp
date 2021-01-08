#include "HFS_DeepenScopeOperation.hpp"

namespace hfs {
    OperationResult DeepenScopeOperation::internal_run(ScriptHolder* holder,
                                                       Scope* const scope,
                                                       const std::vector<Variable>& values,
                                                       Variable* const returned_value,
                                                       Operation** const next_operation,
                                                       Scope** const next_scope) const
    {
        *next_operation = this->next_operation;
        *returned_value = Variable::create_null();
        *next_scope = new Scope(scope);
        return OperationResult::Return;
    }
}