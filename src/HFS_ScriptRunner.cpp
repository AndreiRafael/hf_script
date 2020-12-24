#include "HFS_ScriptRunner.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

#include "./operations/HFS_FunctionCallOperation.hpp"
#include "./utility/HFS_Utility.hpp"

namespace fs = std::filesystem;

namespace hfs {
    ScriptRunner::ScriptRunner() {
        this->scope = new Scope();
    }

    ScriptRunner::~ScriptRunner() {
        delete this->scope;
    }


    bool ScriptRunner::bind_function(std::string function_name, std::function<Variable(std::vector<Variable>)> function, int param_count) {
        if(utility::validate_name(function_name) && get_bound_function(function_name, param_count) == nullptr) {//can be added
            BoundFunctionDef def = { function_name, param_count };
            
            bound_functions.push_back(std::make_pair(def, function));
            bound_operations.push_back(std::make_pair(def, new FunctionCallOperation(function_name, std::vector<Operation*>())));
            return true;
        }
        return false;
    }

    Operation* ScriptRunner::get_script_function(const std::string_view function_name, const unsigned int param_count) {
        for(auto s : scripts) {
            auto f = s->find_function(function_name, param_count);
            if(f != nullptr) {
                return f;
            }
        }
        return nullptr;
    }

    std::function<Variable(std::vector<Variable>)>* ScriptRunner::get_bound_function(const std::string_view function_name, const int param_count) {
        for(int i = 0; i < bound_functions.size(); ++i) {
            const auto def = bound_functions[i].first;
            if(def.name.compare(function_name) == 0 && (def.param_count == param_count || def.param_count == -1)) {
                return &bound_functions[i].second;
            }
        }
        return nullptr;
    }

    std::vector<std::string> ScriptRunner::get_parameter_names(const std::string_view function_name, const unsigned int param_count) {
        for(auto s : scripts) {
            auto v = s->get_function_parameters(function_name, param_count);
            if(v.size() > 0) {
                return v;
            }
        }

        return std::vector<std::string>();
    }

    bool ScriptRunner::add_script(Script* script, std::string* error_string) {
        if(script == nullptr || !script->is_compiled()) {
            return false;
            if(error_string != nullptr) {
                *error_string = "Invalid script";
            }
        }
        scripts.push_back(script);
        return true;
    }
    
    unsigned int ScriptRunner::start_function(const std::string_view function_name, const std::vector<Variable> parameters) {
        bool from_script = true;;
        Operation* first_operation = get_script_function(function_name, parameters.size());
        if(first_operation == nullptr) {//try to get bound op
            from_script = false;
            for(auto op : bound_operations) {
                const auto def = op.first;
                if(def.name.compare(function_name) == 0 && (def.param_count == -1 || def.param_count == parameters.size())) {
                    first_operation = op.second;
                    break;
                }
            }
        }
        if(first_operation == nullptr) {
            return 0u;
        }

        while(++id_gen == 0u || operation_runners.find(id_gen) != operation_runners.end()) {}


        core::OperationRunner* new_runner = new core::OperationRunner();
        Scope* new_scope = new Scope(scope);
        new_runner->setup(first_operation, new_scope);
        if(from_script) {
            auto names = get_parameter_names(function_name, parameters.size());
            for(int i = 0; i < names.size() && i < parameters.size(); ++i) {
                *new_scope->get_variable(names[i]) = parameters[i];
            }
        }
        else {
            new_runner->insert_values(parameters);
        }

        operation_runners.insert(std::make_pair(id_gen, new_runner));
        return id_gen;
    }

    std::vector<ReturnPair> ScriptRunner::step() {
        std::vector<ReturnPair> return_values;
        std::vector<unsigned int> to_remove;

        for(auto& pair : operation_runners) {
            auto& runner = pair.second;
            auto& id = pair.first;
            core::RunnerResult result;
            do {
                result = runner->step(this);
            } while(result == core::RunnerResult::Ongoing);

            if(result == core::RunnerResult::Return) {
                return_values.push_back({ id, runner->get_result() });
                to_remove.push_back(id);
                delete runner;// TODO: Destruir o escopo!
            }
        }
        
        for(auto& id : to_remove) {
            operation_runners.erase(id);
        }

        return return_values;
    }

    Scope* ScriptRunner::get_scope() const {
        return this->scope;
    }
}