#include "HFS_OperationRunner.hpp"

namespace hfs {
    void OperationRunner::clear_data() {
        req_index = 0;

        result = Variable::create_null();
        values.clear();
    }

    void OperationRunner::setup(Operation* const start_operation, Scope* const start_scope) {
        operation = start_operation;
        scope = start_scope;
        clear_data();
    }

    RunnerResult OperationRunner::step() {
        auto requirements = operation->get_requirements();
        if(req_index >= requirements.size()) {
            Scope* next_scope;
            const auto op_result = operation->run(scope, values, &result, &operation, &scope);
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
            const auto child_step = child_runner->step();
            switch (child_step)
            {
            case RunnerResult::Return:
                values.push_back(child_runner->get_result());
                delete child_runner;
                child_runner = nullptr;
                req_index++;
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

    Variable OperationRunner::get_result() {// TODO: This name is very confusing, feels like it is going to return a RunnerResult
        return result;
    }
}