#include "HFS_SetOperation.hpp"

namespace hfs {
    SetOperation::SetOperation(const std::string variable_name, Operation* const value) {
        this->variable_name = variable_name;
        add_requirement(value);
    }

    OperationResult SetOperation::run(Scope* const scope, std::vector<Variable> values, Variable* const value) const {
        auto var = scope->get_variable(variable_name);
        var->set(values[0].get_raw_value());
        if(value != nullptr) {
            *value = *var;
        }
        return OperationResult::Return;
    }
}