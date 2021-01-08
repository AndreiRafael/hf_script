#include "HFS_Operation.hpp"

namespace hfs {
    void Operation::add_requirement(Operation* operation) {
        requirements.push_back(operation);
    }

    void Operation::clear_requirements() {
        requirements.clear();
    }


    Operation::~Operation() {
        for(auto& req : requirements) {
            delete req;
        }
    }

    std::vector<Operation*> Operation::get_requirements() const {
        return requirements;
    }

    OperationResult Operation::run(ScriptHolder* holder,
                                   Scope* const scope,
                                   Variable* const return_value,
                                   Operation** const next_operation,
                                   Scope** const next_scope) const {
        return run(holder, scope, std::vector<Variable>(), return_value, next_operation, next_scope);
    }

    OperationResult Operation::run(ScriptHolder* holder,
                                   Scope* const scope,
                                   const std::vector<Variable>& values,
                                   Variable* const return_value,
                                   Operation** const next_operation,
                                   Scope** const next_scope) const {
        if(holder == nullptr || scope == nullptr || return_value == nullptr || next_operation == nullptr || next_scope == nullptr) {
            return OperationResult::Error;
        }

        return internal_run(holder, scope, values, return_value, next_operation, next_scope);
    }
}