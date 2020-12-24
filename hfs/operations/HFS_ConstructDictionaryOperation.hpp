#ifndef HFS_CONSTRUCTDICTIONARYOPERATION_HPP
#define HFS_CONSTRUCTDICTIONARYOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class ConstructDictionaryOperation : public Operation {
    protected:
        OperationResult internal_run(ScriptRunner* runner,
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const return_value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    public:
        void add_pair(Operation* key_operation, Operation* value_operation);
    };
}

#endif