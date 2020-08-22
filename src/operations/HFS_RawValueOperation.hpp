#ifndef HFS_RAWVALUEOPERATION_HPP
#define HFS_RAWVALUEOPERATION_HPP

#include "HFS_Operation.hpp"

namespace hfs {
    class RawValueOperation : public Operation {
    private:
        std::string value;
    public:
        RawValueOperation(const std::string value);

        OperationResult run(Scope* const scope, std::vector<Variable> values, Variable* const value) const final;
    };
}

#endif