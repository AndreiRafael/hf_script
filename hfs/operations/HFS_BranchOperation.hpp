#ifndef HFS_BRANCHOPERATION_HPP
#define HFS_BRANCHOPERATION_HPP

#include "HFS_Operation.hpp"
#include <array>

namespace hfs {
    class BranchOperation : public Operation {
    private:
        std::array<Operation*, 2> operation_array;

    protected:
        OperationResult internal_run(ScriptHolder* holder,
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const return_value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    public:
        BranchOperation(Operation* const value);

        /**
         * \brief Determines which operation will be next if the required operation returns true
         */
        void set_true_operation(Operation* const operation);

        /**
         * \brief Determines which operation will be next if the required operation returns false
         */
        void set_false_operation(Operation* const operation);
    };
}

#endif//HFS_BRANCHOPERATION_HPP