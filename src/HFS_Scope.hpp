#ifndef HFS_SCOPE_HPP
#define HFS_SCOPE_HPP

#include "HFS_Variable.hpp"
#include <vector>
#include <unordered_map>

namespace hfs {
    class Scope {
    private:
        Scope* parent_scope = nullptr;
        std::unordered_map<std::string, Variable> variables;

    public:
        Scope(Scope* const parent = nullptr);

        /**
         * \brief Gets a variable in this scope or in parent scopes. Creates a new null variable if it does not exist
         * \remarks Will return nullptr if the variable name has size 0 or if the name is "_" (global variable rule)
         */
        Variable* get_variable(const std::string name);

        Scope* get_parent();
    };
}

#endif