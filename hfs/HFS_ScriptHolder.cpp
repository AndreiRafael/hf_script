#include "HFS_ScriptHolder.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

#include "./operations/HFS_FunctionCallOperation.hpp"
#include "./utility/HFS_Utility.hpp"
#include "HFS_Script.hpp"

namespace fs = std::filesystem;

namespace hfs {
    ScriptHolder::ScriptHolder() {
        this->scope = new Scope();
    }

    ScriptHolder::~ScriptHolder() {
        delete this->scope;

        for(auto holder : subordinate_script_holders) {
            holder->remove_superior_holder(this);
        }
    }

    bool ScriptHolder::create_function_runner (const std::string_view function_name,
                                               const std::vector<Variable> parameters,
                                               Scope* top_scope,
                                               core::OperationRunner** new_runner) {
        bool from_script = true;
        Operation* operation = get_script_function(function_name, parameters.size());
        if(operation == nullptr) {//try to get bound op
            from_script = false;
            for(auto op : bound_operations) {
                const auto def = op.first;
                if(def.name.compare(function_name) == 0 && (def.param_count == -1 || def.param_count == parameters.size())) {
                    operation = op.second;
                }
            }
        }
        if(operation == nullptr) {
            for(auto r : superior_script_holders) {
                if(r->create_function_runner(function_name, parameters, top_scope, new_runner)) {
                    return true;
                }
            }
            return false;
        }

        core::OperationRunner* runner = new core::OperationRunner();
        Scope* new_scope = new Scope(top_scope);
        runner->setup(operation, new_scope);
        if(from_script) {
            auto names = get_parameter_names(function_name, parameters.size());
            for(std::size_t i = 0; i < names.size() && i < parameters.size(); ++i) {
                *new_scope->get_variable(names[i]) = parameters[i];
            }
        }
        else {
            runner->insert_values(parameters);
        }

        *new_runner = runner;
        return true;
    }

    bool ScriptHolder::has_superior_holder(ScriptHolder* holder) const {
        for(auto r : superior_script_holders) {
            if(r == holder || r->has_superior_holder(holder)) {
                return true;
            }
        }
        return false;
    }

    bool ScriptHolder::step_runner(core::OperationRunner* runner, unsigned int runner_id, ReturnData* returned_data) {
        core::RunnerResult result;
        int operations_left = operation_limit;
        const bool no_limit = operation_limit <= 0;
        do {
            result = runner->step(this);
        } while(result == core::RunnerResult::Ongoing && (no_limit || --operations_left > 0));

        if(result == core::RunnerResult::Return) {
            *returned_data = { true, runner_id, runner->get_returned_value() };
            delete runner;
            return true;
        }
        else if (!no_limit && operations_left <= 0) {//failed becaused reached operation_limit
            *returned_data = { false, runner_id, Variable::create_null() };
            delete runner;
            return true;
        }
        return false;
    }

    bool ScriptHolder::bind_function(std::string function_name, std::function<Variable(std::vector<Variable>)> function, int param_count) {
        if(utility::validate_name(function_name) && get_bound_function(function_name, param_count) == nullptr) {//can be added
            BoundFunctionDef def = { function_name, param_count };
            
            bound_functions.push_back(std::make_pair(def, function));
            bound_operations.push_back(std::make_pair(def, new FunctionCallOperation(function_name, std::vector<Operation*>())));
            return true;
        }
        return false;
    }

    Operation* ScriptHolder::get_script_function(const std::string_view function_name, const unsigned int param_count) {
        for(auto s : scripts) {
            auto f = s->find_function(function_name, param_count);
            if(f != nullptr) {
                return f;
            }
        }
        return nullptr;
    }

    std::function<Variable(std::vector<Variable>)>* ScriptHolder::get_bound_function(const std::string_view function_name, const int param_count) {
        for(std::size_t i = 0; i < bound_functions.size(); ++i) {
            const auto def = bound_functions[i].first;
            if(def.name.compare(function_name) == 0 && (def.param_count == param_count || def.param_count == -1)) {
                return &bound_functions[i].second;
            }
        }
        return nullptr;
    }

    std::vector<std::string> ScriptHolder::get_parameter_names(const std::string_view function_name, const unsigned int param_count) {
        for(auto s : scripts) {
            auto v = s->get_function_parameters(function_name, param_count);
            if(v.size() > 0) {
                return v;
            }
        }

        return std::vector<std::string>();
    }


    bool ScriptHolder::get_operator(const std::string symbol, const core::OperatorType type, core::Operator* op) const{
        if(validate_operator(symbol)) {
            for(auto o : operators) {
                if(o.symbol.compare(symbol) == 0 && (static_cast<char>(o.type) & static_cast<char>(type)) != 0) {
                    if(op != nullptr) {
                        *op = o;
                    }
                    return true;
                }
            }
        }

        if(op != nullptr) {
            *op = { "", "", core::OperatorType::None };
        }
        return false;
    }

    bool ScriptHolder::define_operator(const std::string symbol, const std::string function, const core::OperatorType type) {
        if(type == core::OperatorType::None || type == core::OperatorType::Any || !validate_operator(symbol) || get_operator(symbol, type, nullptr)) {
            return false;// TODO: push error logs for each error type
        }

        core::Operator o = { symbol, function, type };
        operators.push_back(o);
        return true;
    }

    bool ScriptHolder::undefine_operator(const std::string symbol, const core::OperatorType type) {
        auto comp = [&] (core::Operator const& op) { return op.type == type && op.symbol.compare(symbol) == 0; };
        auto itr = std::find_if(operators.begin(), operators.end(), comp);
        if(itr != operators.end()) {
            operators.erase(std::find_if(operators.begin(), operators.end(), comp)); 
            return true;
        }
        return false;
    }

    bool ScriptHolder::validate_operator(std::string const& symbol) {
        if(symbol.size() == 0 || symbol.compare("=") == 0) {
            return false;
        }

        return std::all_of(symbol.begin(), symbol.end(), [] (char const& c) { return utility::is_operator_symbol(c); });
    }

    bool ScriptHolder::add_script(Script* script, std::string* error_string) {
        if(script == nullptr || !script->is_compiled()) {
            if(error_string != nullptr) {
                *error_string = "Invalid script";
            }
            return false;
        }
        auto itr = std::find(scripts.begin(), scripts.end(), script);
        if(itr != scripts.end()) {//cannot add same script pointer twice
            script->associated_holders.push_back(this);//adds self to associated runners of this script
            scripts.push_back(script);
            return true;
        }
        return false;
    }

    bool ScriptHolder::remove_script(Script* script) {
        if(script != nullptr) {
            auto itr = std::find(scripts.begin(), scripts.end(), script);
            if(itr != scripts.end()) {
                scripts.erase(itr);
                return true;
            }
        }
        return false;
    }

    bool ScriptHolder::add_superior_holder(ScriptHolder* holder) {
        if(holder == nullptr || holder->has_superior_holder(this)) {
            return false;
        }

        superior_script_holders.push_back(holder);
        holder->subordinate_script_holders.push_back(this);
        return true;
    }

    bool ScriptHolder::remove_superior_holder(ScriptHolder* holder) {
        auto itr = std::find(superior_script_holders.begin(), superior_script_holders.end(), holder);
        if(itr != superior_script_holders.end()) {
            superior_script_holders.erase(itr);
            return true;
        }
        return false;
    }

    void ScriptHolder::set_operation_limit(const int amount) {
        operation_limit = amount;
    }
    
    unsigned int ScriptHolder::start_function(const std::string_view function_name, const std::vector<Variable> parameters) {
        core::OperationRunner* new_runner;
        if(!create_function_runner(function_name, parameters, this->scope, &new_runner)) {
            return 0u;
        }

        while(++id_gen == 0u || operation_runners.find(id_gen) != operation_runners.end()) {}

        ReturnData returned_data = {false, 0, Variable::create_null() };
        if(step_runner(new_runner, id_gen, &returned_data)) {//first step. If terminated, adds value ro results, else justs adds new runner to list
            return_data.push_back(returned_data);
        }
        else {
            operation_runners.insert(std::make_pair(id_gen, new_runner));
        }
        return id_gen;
    }

    std::vector<ReturnData> ScriptHolder::step() {
        return_data.clear();
        std::vector<unsigned int> to_remove;

        for(auto& pair : operation_runners) {
            auto& runner = pair.second;
            auto& id = pair.first;
            ReturnData returned_data = {false, 0, Variable::create_null() };
            
            if(step_runner(runner, id, &returned_data)) {
                return_data.push_back(returned_data);
                to_remove.push_back(id);
            }
        }
        
        for(auto& id : to_remove) {
            operation_runners.erase(id);
        }

        return return_data;
    }

    bool ScriptHolder::run_function(std::string_view function_name, const std::vector<Variable> parameters, Variable* returned_value) {
        core::OperationRunner* runner;
        if(!create_function_runner(function_name, parameters, this->scope, &runner)) {
            return false;
        }

        ReturnData returned_data = {false, 0, Variable::create_null() };
        while(!step_runner(runner, 0, &returned_data)) {}
        
        *returned_value = returned_data.value;
        return returned_data.success;
    }

    bool ScriptHolder::get_returned_data(unsigned int function_id, ReturnData* returned_data) {
        auto itr = std::find_if(return_data.begin(), return_data.end(), [&function_id] (ReturnData const& data) -> bool { return data.id == function_id; });
        if(itr != return_data.end()) {
            *returned_data = *itr;
            return true;
        }
        return false;
    }

    Scope* ScriptHolder::get_scope() const {
        return this->scope;
    }
}