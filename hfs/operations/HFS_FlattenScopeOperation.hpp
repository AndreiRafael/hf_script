#ifndef HFS_FLATTENSCOPEOPERATION_HPP
#define HFS_FLATTENSCOPEOPERATION_HPP

#include "HFS_SequentialOperation.hpp"

namespace hfs {
    class FlattenScopeOperation : public SequentialOperation {
    protected:
        OperationResult internal_run(ScriptHolder* holder,
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const returned_value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    };
}

#endif//HFS_FLATTENSCOPEOPERATION_HPP