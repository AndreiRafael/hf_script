#ifndef HFS_VARIABLERETRIALVEOPERATION_HPP
#define HFS_VARIABLERETRIALVEOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class VariableRetrievalOperation : public Operation {
    private:
        std::string variable_name = "";

    protected:
        OperationResult internal_run(ScriptHolder* holder,
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const returned_value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    public:
        VariableRetrievalOperation(const std::string variable_name);

        void add_dictionary_key(Operation* key_op);

        std::string get_variable_name() const;
    };
}

#endif//HFS_VARIABLERETRIALVEOPERATION_HPP