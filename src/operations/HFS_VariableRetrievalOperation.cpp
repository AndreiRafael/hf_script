#include "HFS_VariableRetrievalOperation.hpp"

namespace hfs {
    VariableRetrievalOperation::VariableRetrievalOperation(const std::string variable_name) {
        this->variable_name = variable_name;
    }
    
    OperationResult VariableRetrievalOperation::internal_run(ScriptRunner* runner,
                                                             Scope* const scope,
                                                             const std::vector<Variable>& values,
                                                             Variable* const returned_value,
                                                             Operation** const next_operation,
                                                             Scope** const next_scope) const {
        *next_operation = nullptr;
        *next_scope = scope;

        auto var = scope->get_variable(variable_name);
        if(var != nullptr) {
            Variable composedValue = *var;
            for(int i = 0; i < values.size(); ++i) {
                auto entry = composedValue.get_dictionary_entry(values[i].get_raw_value());
                if(entry == nullptr) {
                    composedValue = Variable::create_null();
                    break;
                }
                composedValue = *entry;
            }
            *returned_value = composedValue;
        }
        else {
            *returned_value = Variable::create_null();
        }

        return OperationResult::Return;
    }

    void VariableRetrievalOperation::add_dictionary_key(Operation* key_op) {
        add_requirement(key_op);
    }

    std::string VariableRetrievalOperation::get_variable_name() const {
        return variable_name;
    }
}