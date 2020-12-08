#ifndef HFS_OPERATIONRUNNER_HPP
#define HFS_OPERATIONRUNNER_HPP

#include "operations/HFS_Operation.hpp"
#include "HFS_Variable.hpp"

namespace hfs {
    enum class RunnerResult {
        Ongoing,//did not finish execution, can call step again immediatly
        Wait,   //did not finish execution, but was told to wait
        Return  //finished execution, guaranteeing get_result returns a valid variable
    };
    
    class OperationRunner {
    private:
        int req_index = 0;
        OperationRunner* child_runner = nullptr;

        Operation* operation;
        Scope* scope;
        Variable result = Variable::create_null();

        std::vector<Variable> values;

    public:
        void setup(Operation* const start_operation, Scope* const scope);

        /**
         * \brief Steps the operator once, returns the result of the step and fills \param value if finished running
         */
        RunnerResult step();

        /**
         * \brief Gets the result of running the operation, only valid after \ref returns RunnerResult::Return
         */
        Variable get_result();
    };
}

#endif