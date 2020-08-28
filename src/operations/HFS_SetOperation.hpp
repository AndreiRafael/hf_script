#ifndef HFS_SETOPERATION_HPP
#define HFS_SETOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    /**
     * \brief Given a name of a variable, sets that to a value. Will return the set value
     */
    class SetOperation : public Operation {
    private:
        std::string variable_name;
    public:
        SetOperation(const std::string variable_name, Operation* const value);

        OperationResult run(Scope* const scope,
                            std::vector<Variable> values,
                            Variable* const return_value,
                            Operation** const next_operation,
                            Scope** next_scope) const final;
    };
}

#endif