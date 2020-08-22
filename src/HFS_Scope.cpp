#include "HFS_Scope.hpp"

namespace hfs {
    Scope::Scope(Scope* const parent) {
        parent_scope = parent;
    }

    Variable* Scope::get_variable(const std::string name) {
        if(name.size() < 1 || name.compare("_") == 0) {
            return nullptr;
        }

        auto starting_scope = this;
        if(name[0] == '_') {//global rule
            while(starting_scope->parent_scope != nullptr) {
                starting_scope = starting_scope->parent_scope;
            }
        }

        auto lookup_scope = starting_scope;

        while(lookup_scope != nullptr) {
            auto itr = lookup_scope->variables.find(name);
            if(itr != lookup_scope->variables.end()) {
                return &itr->second;
            }
            lookup_scope = lookup_scope->parent_scope;
        }
        
        starting_scope->variables.insert(std::make_pair(name, Variable::create_null()));
        return get_variable(name);
    }
}