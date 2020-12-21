#include "HFS_SetOperation.hpp"

namespace hfs {
    SetOperation::SetOperation(VariableRetrievalOperation* const retrieval_op, Operation* const value) {
        this->retrieval_op = retrieval_op;
        auto dictionary_keys = retrieval_op->get_requirements();
        for(auto key : dictionary_keys) {
            add_requirement(key);
        }
        add_requirement(value);
    }

    OperationResult SetOperation::internal_run(ScriptRunner* runner,
                                               Scope* const scope,
                                               const std::vector<Variable>& values, 
                                               Variable* const returned_value,
                                               Operation** const next_operation,
                                               Scope** const next_scope) const {
        if(values.size() > 0) {
            auto var = scope->get_variable(retrieval_op->get_variable_name());
            auto value = Variable::create_copy(values[values.size() - 1]);

            for(auto i = 0; i < values.size() -1; ++i) {
                var = var->get_or_create_dictionary_entry(values[i].get_raw_value(), value);
            }

            *var = value;

            *returned_value = *var;
        }
        else {
            *returned_value = Variable::create_null();
        }
        *next_operation = this->next_operation;
        *next_scope = scope;

        return OperationResult::Return;
    }
}