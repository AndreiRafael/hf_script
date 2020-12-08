#include "HFS_SetOperation.hpp"

namespace hfs {
    SetOperation::SetOperation(const std::string variable_name, Operation* const value) : SetOperation::SetOperation(variable_name, value, std::vector<Operation*>(0)) {}

    SetOperation::SetOperation(const std::string variable_name, Operation* const value, std::vector<Operation*> dictionary_keys) {
        this->variable_name = variable_name;
        for(auto key : dictionary_keys) {
            add_requirement(key);
        }
        add_requirement(value);
    }

    OperationResult SetOperation::run(Scope* const scope, std::vector<Variable> values, Variable* const value, Operation** const next_operation, Scope** next_scope) const {
        if(values.size() > 0) {
            auto var = scope->get_variable(variable_name);
            auto composed_value = Variable::create_copy(values[values.size() - 1]);
            for(int i = values.size() - 2; i >= 0; --i) {
                composed_value = Variable::create_dictionary(values[i].get_raw_value(), Variable::create_copy(composed_value));
            }

            var->copy(composed_value);

            *value = *var;
        }
        else {
            *value = Variable::create_null();
        }
        *next_operation = this->next_operation;
        *next_scope = scope;

        return OperationResult::Return;
    }
}