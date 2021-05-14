#include "HFS_OperatorOperation.hpp"
#include "../HFS_ScriptHolder.hpp"

namespace hfs {
    OperatorOperation::OperatorOperation(const std::string operator_symbol, Operation* single_value) {
        this->operator_symbol = operator_symbol;
        add_requirement(single_value);
    }

    OperatorOperation::OperatorOperation(const std::string operator_symbol, Operation* left_value, Operation* right_value) {
        this->operator_symbol = operator_symbol;
        add_requirement(left_value);
        add_requirement(right_value);
    }

    OperationResult OperatorOperation::internal_run(ScriptHolder* holder,
                                                        Scope* const scope,
                                                        const std::vector<Variable>& values, 
                                                        Variable* const returned_value,
                                                        Operation** const next_operation,
                                                        Scope** const next_scope) const {
        
        core::Operator* op = nullptr;
        if(!holder->get_operator(operator_symbol, values.size() == 1 ? core::OperatorType::Single : core::OperatorType::Double, op)) {
            return OperationResult::Error;// TODO: runtime error!!
        }

        auto function_name = op->function;
        auto script_function = holder->get_script_function(function_name, values.size());
        if(script_function != nullptr) {//wil run script function
            Scope* new_scope = new Scope(holder->get_scope());
            auto names = holder->get_parameter_names(function_name, values.size());
            for(int i = 0; i < names.size() && i < values.size(); ++i) {
                *new_scope->get_variable(names[i]) = values[i];
            }

            *next_operation = script_function;
            *next_scope = new_scope;
            *returned_value = Variable::create_null();
            return OperationResult::Return;
        }

        auto bound_function = holder->get_bound_function(function_name, values.size());
        if(bound_function != nullptr) {
            *returned_value = (*bound_function)(values);
            *next_operation = nullptr;
            *next_scope = nullptr;
            return OperationResult::Return;
        }

        *returned_value = Variable::create_null();
        return OperationResult::Return;
    }
}