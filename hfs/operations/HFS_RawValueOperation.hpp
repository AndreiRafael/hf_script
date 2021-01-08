#ifndef HFS_RAWVALUEOPERATION_HPP
#define HFS_RAWVALUEOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class RawValueOperation : public Operation {
    private:
        std::string value;

    protected:
        OperationResult internal_run(ScriptHolder* holder,
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    public:
        RawValueOperation(const std::string value);
    };
}

#endif