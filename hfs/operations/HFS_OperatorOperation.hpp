#ifndef HFS_OPERATOROPERATION_HPP
#define HFS_OPERATOROPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class OperatorOperation : public Operation {
    private:
        std::string operator_symbol;

    protected:
        OperationResult internal_run(ScriptHolder* holder,
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const returned_value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    public:
        OperatorOperation(const std::string operator_symbol, Operation* single_value);
        OperatorOperation(const std::string operator_symbol, Operation* left_value, Operation* right_value);
    };
}

#endif