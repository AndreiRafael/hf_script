#ifndef HFS_FUNCTIONOPERATION_HPP
#define HFS_FUNCTIONOPERATION_HPP

#include "HFS_Operation.hpp"
#include "../HFS_ScriptRunner.hpp"

namespace hfs {
    class FunctionCallOperation : public Operation {
    private:
        std::string function_name;
        ScriptRunner* runner;
    public:
        FunctionCallOperation(const std::string function_name, ScriptRunner* const runner);

        OperationResult run(Scope* const scope,
                            const std::vector<Variable> values,
                            Variable* const returned_value,
                            Operation** const next_operation,
                            Scope** next_scope) const final;
    };
}

#endif