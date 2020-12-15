#include "HFS_VariableRetrieveOperation.hpp"

namespace hfs {
    VariableRetrieveOperation::VariableRetrieveOperation(const std::string variable_name) {
        this->variable_name = variable_name;
    }
    
    OperationResult VariableRetrieveOperation::internal_run(Scope* const scope,
                                                            const std::vector<Variable>& values,
                                                            Variable* const returned_value,
                                                            Operation** const next_operation,
                                                            Scope** const next_scope) const {
        *next_operation = nullptr;
        *next_scope = scope;

        auto var = scope->get_variable(variable_name);
        if(var != nullptr) {
            *returned_value = Variable::create_copy(*var);
            return OperationResult::Return;
        } 

        *returned_value = Variable::create_null();
        return OperationResult::Return;
    }
}