#ifndef HFS_SCRIPTLINE_HPP
#define HFS_SCRIPTLINE_HPP

#include "../operations/HFS_Operation.hpp"
#include <stack>

namespace hfs {
    enum class ScriptLineResult {
        Finished,
        Returned,
        Wait
    };

    class ScriptLine {
    private:
        Operation* operation;
    public:
        ScriptLine(Operation* const operation);

        virtual ScriptLineResult run(Scope* const start_scope,  Scope* exit_scope);
    };
}

#endif