#ifndef HFS_VARIABLERETRIEVEOPERATION_HPP
#define HFS_VARIABLERETRIEVEOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class VariableRetrieveOperation : public Operation {
    private:
        std::string variable_name = "";

    protected:
        OperationResult internal_run(Scope* const scope,
                            const std::vector<Variable>& values,
                            Variable* const returned_value,
                            Operation** const next_operation,
                            Scope** const next_scope) const final;
    public:
        VariableRetrieveOperation(const std::string variable_name);
    };
}

#endif