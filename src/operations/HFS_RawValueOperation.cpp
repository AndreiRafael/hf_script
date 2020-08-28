#include "HFS_RawValueOperation.hpp"

namespace hfs {
    RawValueOperation::RawValueOperation(const std::string value) {
        this->value = value;
    }

    OperationResult RawValueOperation::run(Scope* const scope, std::vector<Variable> values, Variable* const value, Operation** const next_operation, Scope** next_scope) const {
        
        *value = Variable::create(this->value);
        *next_operation = nullptr;
        *next_scope = scope;
    
        return OperationResult::Return;
    }
}