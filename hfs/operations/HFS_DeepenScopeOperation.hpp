#ifndef HFS_DEEPENSCOPEOPERATION_HPP
#define HFS_DEEPENSCOPEOPERATION_HPP

#include "HFS_SequentialOperation.hpp"

namespace hfs {
    /**
     * \brief Creates a new scope, having the ran scope as parent
     * \sa hfs::FlattenScopeOperation 
     */
    class DeepenScopeOperation : public SequentialOperation {
    protected:
        OperationResult internal_run(ScriptRunner* runner,
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const returned_value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    };
}

#endif//HFS_DEEPENSCOPEOPERATION_HPP