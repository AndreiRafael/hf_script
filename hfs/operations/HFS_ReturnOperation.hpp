#ifndef HFS_RETURNOPERATION_HPP
#define HFS_RETURNOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class ReturnOperation : public Operation {
    protected:
        OperationResult internal_run(ScriptHolder* holder,
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const returned_value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    public:
        ReturnOperation(Operation* operation);
    };
}

#endif//HFS_RETURNOPERATION_HPP