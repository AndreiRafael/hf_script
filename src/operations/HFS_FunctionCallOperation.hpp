#ifndef HFS_FUNCTIONOPERATION_HPP
#define HFS_FUNCTIONOPERATION_HPP

#include "HFS_Operation.hpp"
#include "./HFS_ScriptRunner.hpp"

namespace hfs {
    class FunctionCallOperation : public Operation {
    private:
        std::string function_name;
        std::vector<Operation*> parameters;
        ScriptRunner* runner;
    public:
        FunctionCallOperation(const std::string function_name, const std::vector<Operation*> parameters, ScriptRunner* const runner);

        Variable run(Scope* const scope) const;
    };
}

#endif