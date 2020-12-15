#include "HFS_RawValueOperation.hpp"

namespace hfs {
    RawValueOperation::RawValueOperation(const std::string value) {
        this->value = value;
    }

    OperationResult RawValueOperation::internal_run(Scope* const scope,
                                                    const std::vector<Variable>& values,
                                                    Variable* const value,
                                                    Operation** const next_operation,
                                                    Scope** const next_scope) const {
        
        *value = Variable::create(this->value);
        *next_operation = nullptr;
        *next_scope = scope;
    
        return OperationResult::Return;
    }
}