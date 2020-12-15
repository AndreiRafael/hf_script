#include "HFS_WaitOperation.hpp"
#include <algorithm>

namespace hfs {
    OperationResult WaitOperation::internal_run(Scope* const scope,
                                       const std::vector<Variable>& values,
                                       Variable* const return_value,
                                       Operation** const next_operation,
                                       Scope** const next_scope) const
    {
        if(std::all_of(values.begin(), values.end(), [] (const Variable& var) { return var.get_type() == VariableType::Boolean && var.get_boolean_value();})) {
            return OperationResult::Return;
        }
        return OperationResult::Wait;
        

        //TODO: setar valores de retorno/proxima operação/escopo
        *next_operation = this->next_operation;
    }
}