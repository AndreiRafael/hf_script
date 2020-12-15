#ifndef HFS_RELEASEOPERATION_HPP
#define HFS_RELEASEOPERATION_HPP

#include "HFS_SequentialOperation.hpp"

namespace hfs {
    /**
     * @brief An operation that does nothing, but tells the runner to hold code execution
     */
    class ReleaseOperation : public SequentialOperation {
    protected:
        OperationResult internal_run(Scope* const scope,
                            const std::vector<Variable>& values,
                            Variable* const return_value,
                            Operation** const next_operation,
                            Scope** const next_scope) const final;
    };
}

#endif//HFS_RELEASEOPERATION_HPP