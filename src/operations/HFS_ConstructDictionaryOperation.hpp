#ifndef HFS_CONSTRUCTDICTIONARYOPERATION_HPP
#define HFS_CONSTRUCTDICTIONARYOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class ConstructDictionaryOperation : public Operation {
        OperationResult run(Scope* const scope,
                            std::vector<Variable> values,
                            Variable* const return_value,
                            Operation** const next_operation,
                            Scope** next_scope) const final;

        void add_pair(Operation* key_operation, Operation* value_operation);
    };
}

#endif