#ifndef HFS_OPERATION_HPP
#define HFS_OPERATION_HPP

#include "../HFS_Scope.hpp"
#include <vector>

namespace hfs {
    class ScriptHolder;

    enum class OperationResult {
        Error,  //something went wrong, should stop and inform that
        Return, //returned a value, generally breaking the execution of subsequent code
        Wait    //stopped executing by finding a wait operation
    };

    /**
     * \brief Operations are things that can be executed on a scope, be it a simple function call or a while loop
     */
    class Operation {
    private:
        std::vector<Operation*> requirements = std::vector<Operation*>(0);

    protected:
        void add_requirement(Operation* operation);
        void clear_requirements();

        virtual OperationResult internal_run(ScriptHolder* holder,
                                    Scope* const scope,
                                    const std::vector<Variable>& values,
                                    Variable* const returned_value,
                                    Operation** const next_operation,
                                    Scope** const next_scope) const = 0;
    public:
        virtual ~Operation();

        /**
         * @brief Gets the requirements that this Operation needs to complete before calling \ref run
         */
        std::vector<Operation*> get_requirements() const;

        /**
         * @brief Executes this operation on a scope
         * @param scope The scope to execute this operation in
         * @param returned_value A pointer to a Variable, filled if function returns OperationResult::Return
         * @param next_operation A pointer to be filled with the operation to be executed after this one
         * @param next_scope A pointer to be filled with the pointer to the scope to run the next operation.
         */
        OperationResult run(ScriptHolder* holder,
                            Scope* const scope,
                            const std::vector<Variable>& values,
                            Variable* const returned_value,
                            Operation** const next_operation,
                            Scope** const next_scope) const;
        OperationResult run(ScriptHolder* holder,
                            Scope* const scope,
                            Variable* const returned_value,
                            Operation** const next_operation,
                            Scope** const next_scope) const;
    };
}

#endif