#include "HFS_RawValueOperation.hpp"

namespace hfs {
    RawValueOperation::RawValueOperation(const std::string value) {
        this->value = value;
    }

    OperationResult RawValueOperation::run(Scope* const scope, std::vector<Variable> values, Variable* const value) const {
        if(value != nullptr) {
            *value = Variable::create(this->value);
        }
        return OperationResult::Return;
    }
}