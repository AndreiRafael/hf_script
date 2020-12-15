#include "HFS_ConstructDictionaryOperation.hpp"

namespace hfs{
    OperationResult ConstructDictionaryOperation::internal_run(Scope* const scope,
                                                               const std::vector<Variable>& values,
                                                               Variable* const return_value,
                                                               Operation** const next_operation,
                                                               Scope** const next_scope) const {
        std::vector<std::string> new_keys;
        std::vector<Variable> new_values;

        for(int i = 0; (i + 1) < values.size(); i += 2) {
            auto key = values[i].get_raw_value();
            auto value = values[i + 1];

            new_keys.push_back(key);
            new_values.push_back(value);
        }

        *return_value = Variable::create_dictionary(new_keys, new_values);
        *next_operation = nullptr;
        *next_scope = nullptr;
        return OperationResult::Return;
    }

    void ConstructDictionaryOperation::add_pair(Operation* key_operation, Operation* value_operation) {
        add_requirement(key_operation);
        add_requirement(value_operation);
    }
}