#ifndef HFS_OPERATION_HPP
#define HFS_OPERATION_HPP

#include "../HFS_Scope.hpp"
#include <vector>

namespace hfs {
    enum class OperationResult {
        Return, //returned a value, generally breaking the execution of subsequent code
        Wait    //stopped executing by finding a wait operation
    };

    /**
     * \brief Operations are things that can be executed on a scope, be it a simple function call or a while loop
     */
    class Operation {
    private:
        std::vector<Operation*> requirements;
    public:
        std::vector<Operation*> add_requirement(Operation* operation);

        std::vector<Operation*> get_requirements() const;
        virtual Operation* get_next_operation() const;

        /**
         * \brief Executes this operation on a scope
         * \param scope The scope to execute this operation in
         * \param returned_value a pointer to a Variable, filled if function returns OperationResult::Return
         */
        virtual OperationResult run(Scope* const scope, std::vector<Variable> values, Variable* const returned_value) const = 0;
        OperationResult run(Scope* const scope, Variable* const returned_value) const;
    };
}

#endif