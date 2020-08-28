#ifndef HFS_IFOPERATION_HPP
#define HFS_IFOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class IfOperation : public Operation {
    private:
        Operation* true_operation;
        Operation* false_operation;
    public:
        IfOperation(const std::string variable_name, Operation* const value);

        OperationResult run(Scope* const scope,
                            std::vector<Variable> values,
                            Variable* const return_value,
                            Operation** const next_operation,
                            Scope** next_scope) const final;

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

#endif//HFS_IFOPERATION_HPP