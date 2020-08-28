#include "HFS_SetOperation.hpp"

namespace hfs {
    SetOperation::SetOperation(const std::string variable_name, Operation* const value) {
        this->variable_name = variable_name;
        add_requirement(value);
    }

    OperationResult SetOperation::run(Scope* const scope, std::vector<Variable> values, Variable* const value, Operation** const next_operation, Scope** next_scope) const {
        auto var = scope->get_variable(variable_name);
        var->set(values[0].get_raw_value());

        *value = *var;
        *next_operation = nullptr;
        *next_scope = scope;

        return OperationResult::Return;
    }
}