#ifndef HFS_SETOPERATION_HPP
#define HFS_SETOPERATION_HPP

#include "HFS_SequentialOperation.hpp"

namespace hfs {
    /**
     * \brief Given a name of a variable, sets that to a value. Will return the set value.
     * \remarks To set Dictionary values, the key should be passed as a requirement. The last requirement will always be the value to set.
     */
    class SetOperation : public SequentialOperation {
    private:
        std::string variable_name;
    public:
        SetOperation(const std::string variable_name, Operation* const value);
        SetOperation(const std::string variable_name, Operation* const value, std::vector<Operation*> dictionary_keys);

        OperationResult run(Scope* const scope,
                            std::vector<Variable> values,
                            Variable* const return_value,
                            Operation** const next_operation,
                            Scope** next_scope) const final;
    };
}

#endif