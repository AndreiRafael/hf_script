#ifndef HFS_SETOPERATION_HPP
#define HFS_SETOPERATION_HPP

#include "HFS_SequentialOperation.hpp"
#include "HFS_VariableRetrievalOperation.hpp"

namespace hfs {
    /**
     * \brief Given a name of a variable, sets that to a value. Will return the set value.
     * \remarks To set Dictionary values, the key should be passed as a requirement. The last requirement will always be the value to set.
     */
    class SetOperation : public SequentialOperation {
    private:
        VariableRetrievalOperation* retrieval_op;

    protected:
        OperationResult internal_run(ScriptHolder* holder, 
                                     Scope* const scope,
                                     const std::vector<Variable>& values,
                                     Variable* const returned_value,
                                     Operation** const next_operation,
                                     Scope** const next_scope) const final;
    public:
        SetOperation(VariableRetrievalOperation* const retrieval_op, Operation* const value);
        virtual ~SetOperation();
    };
}

#endif