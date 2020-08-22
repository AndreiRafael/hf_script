#include "HFS_Operation.hpp"

namespace hfs {
    std::vector<Operation*> Operation::add_requirement(Operation* operation) {
        requirements.push_back(operation);
    }

    Operation* Operation::get_next_operation() const {
        return nullptr;
    }

    std::vector<Operation*> Operation::get_requirements() const {
        return requirements;
    }

    OperationResult Operation::run(Scope* const scope, Variable* const return_value) const {
        return run(scope, std::vector<Variable>(), return_value);
    }
}