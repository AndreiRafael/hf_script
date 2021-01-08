#ifndef HFS_FUNCTIONOPERATION_HPP
#define HFS_FUNCTIONOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class FunctionCallOperation : public Operation {
    private:
        std::string function_name;

    protected:
        OperationResult internal_run(ScriptHolder* holder,
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const returned_value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    public:
        FunctionCallOperation(const std::string function_name, std::vector<Operation*> parameters);
    };
}

#endif