#include "HFS_OperationRunner.hpp"

namespace hfs::core {
    void OperationRunner::clear_data() {
        returned_value = Variable::create_null();
        values.clear();
    }

    void OperationRunner::setup(Operation* const start_operation, Scope* const start_scope) {
        operation = start_operation;
        scope = start_scope;
        clear_data();
    }

    void OperationRunner::insert_values(std::vector<Variable> values) {
        for(auto v : values) {
            this->values.push_back(v);
        }
    }

    RunnerResult OperationRunner::step(ScriptRunner* runner) {
        auto requirements = operation->get_requirements();
        auto req_index = values.size();
        if(req_index >= requirements.size()) {
            Scope* next_scope;
            const auto op_result = operation->run(runner, scope, values, &returned_value, &operation, &scope);
            switch (op_result)
            {
            case OperationResult::Return://when an operation returns, go to the next one. Return if this is the last one
                if(operation == nullptr) {
                    return RunnerResult::Return;
                }
                clear_data();
                return RunnerResult::Ongoing;
            case OperationResult::Wait://operation asked us to wait...
                return RunnerResult::Wait;
            default://ERROR
                return RunnerResult::Error;
            }
        }

        if(child_runner != nullptr) {
            const auto child_step = child_runner->step(runner);
            switch (child_step)
            {
            case RunnerResult::Return:
                values.push_back(child_runner->get_returned_value());
                delete child_runner;
                child_runner = nullptr;
                return RunnerResult::Ongoing;
            case RunnerResult::Error:
                return RunnerResult::Error;
            default:
                return child_step;
            }
        }

        auto req = requirements[req_index];
        child_runner = new OperationRunner();
        child_runner->setup(req, scope);
        return RunnerResult::Ongoing;
    }

    Variable OperationRunner::get_returned_value() {
        return returned_value;
    }
}