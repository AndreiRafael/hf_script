#include "HFS_ReleaseOperation.hpp"
#include <algorithm>

namespace hfs {
    OperationResult ReleaseOperation::internal_run(ScriptRunner* runner,
                                                   Scope* const scope,
                                                   const std::vector<Variable>& values,
                                                   Variable* const returned_value,
                                                   Operation** const next_operation,
                                                   Scope** const next_scope) const
    {
        *next_scope = scope;
        *returned_value = Variable::create_null();
        *next_operation = this->next_operation;

        return OperationResult::Wait;
    }
}