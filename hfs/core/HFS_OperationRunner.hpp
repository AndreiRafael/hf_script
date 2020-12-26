#ifndef HFS_OPERATIONRUNNER_HPP
#define HFS_OPERATIONRUNNER_HPP

#include "../operations/HFS_Operation.hpp"
#include "../HFS_Variable.hpp"

namespace hfs {
    class ScriptRunner;

    namespace core {
        enum class RunnerResult {
            Error,  //something wrong happened, cannot continue execution
            Ongoing,//did not finish execution, can call step again immediatly
            Wait,   //did not finish execution, but was told to wait
            Return  //finished execution, guaranteeing get_returned_value returns a valid variable
        };
        
        class OperationRunner {
        private:
            OperationRunner* child_runner = nullptr;

            Operation* operation;
            Scope* scope;
            Variable returned_value = Variable::create_null();

            std::vector<Variable> values;

            void clear_data();

        public:
            void setup(Operation* const start_operation, Scope* const scope);
            void insert_values(std::vector<Variable> values);

            /**
             * \brief Steps the operator once, returns the result of the step and fills \param value if finished running
             */
            RunnerResult step(ScriptRunner* runner);

            /**
             * \brief Gets the result of running the operation, only valid after \ref OperationRunner::step returns RunnerResult::Return
             */
            Variable get_returned_value();
        };
    }
}

#endif