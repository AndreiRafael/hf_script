#include "HFS_ScriptRunner.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

namespace hfs {
    Operation* ScriptRunner::get_script_function(const std::string_view function_name) {
        auto script_itr = script_functions.find(function_name.data());
        if(script_itr != script_functions.end()) {
            return script_itr->second;
        }
        return nullptr;
    }

    std::function<Variable(std::vector<Variable>)>* ScriptRunner::get_bound_function(const std::string_view function_name) {
        auto bound_itr = bound_functions.find(function_name.data());
        if(bound_itr != bound_functions.end()) {
            return &bound_itr->second;
        }
        return nullptr;
    }

    std::vector<std::string> ScriptRunner::get_variable_names(const std::string_view function_name) {
        auto itr = function_parameters.find(function_name.data());
        if(itr != function_parameters.end()) {
            return itr->second;
        }

        return std::vector<std::string>();
    }

    std::string ScriptRunner::load_script(std::string_view file_path) {
        fs::path path(file_path.data());

        if(fs::is_regular_file(path)) {//only reads regular files
            std::ifstream file_stream(path);
            std::string full_file;

            while(file_stream) {
                std::string line;
                std::getline(file_stream, line);

                auto end_index = line.find('#');//find start of a comment
                full_file.append(line.substr(0, end_index));
            }

            //remove unrequired whitespace
            std::string compact_file;
            std::string compacter_token;
            for(const auto& c : full_file) {
                if(c == ' '
                   || c == static_cast<char>(9)
                   || c == '\n'
                   || c == '\r') {//remove unwanted spaces and tabs
                    if(compacter_token.compare("func") == 0 ||
                       compacter_token.compare("wait") == 0 ||
                       compacter_token.compare("delete") == 0 ||
                       compacter_token.compare("else") == 0 ||
                       compacter_token.compare("return") == 0) {
                        compact_file += ' ';
                    }
                    compacter_token = "";
                    continue;
                }

                compact_file += c;


                if(c == '(' || c == ')'
                || c == '{' || c == '}') {
                    if(compact_file[compact_file.size() - 2] == ' '){
                        compact_file.erase(compact_file.size() - 2, 1);
                    }
                    compacter_token = "";
                    continue;
                }
                
                compacter_token += c;
            }

            
            for(const auto& c : compact_file) {

            }

            std::cout << "Texto lido: " << compact_file << '\n';
        }




        return std::string("sucessfully compiled!");
    }
    
    unsigned int ScriptRunner::start_function(const std::string_view function_name, const std::vector<Variable> parameters) {
        Operation* first_operation = get_script_function(function_name);
        if(first_operation == nullptr) {
            return 0;
        }

        id_gen++;
        while(id_gen == 0u || operation_runners.find(id_gen) != operation_runners.end()) {
            id_gen++;
        }


        OperationRunner* new_runner = new OperationRunner();
        // TODO: Give users the ability to have "global" Scopes
        Scope* new_scope = new Scope();//TODO: we need a way to get variable names here
        // TODO: Additionally, we need to dispose of the newly created Scope
        new_runner->setup(first_operation, new_scope);

        operation_runners.insert(std::make_pair(id_gen, new_runner));
        return id_gen;
    }

    std::vector<std::pair<unsigned int, Variable>> ScriptRunner::step() {
        std::vector<std::pair<unsigned int, Variable>> return_values;
        std::vector<unsigned int> to_remove;

        for(auto& pair : operation_runners) {
            auto& runner = pair.second;
            auto& id = pair.first;
            RunnerResult result;
            do {
                result = runner->step();
            } while(result == RunnerResult::Ongoing);

            if(result == RunnerResult::Return) {
                return_values.push_back(std::make_pair(id, runner->get_result()));
                to_remove.push_back(id);
                delete runner;// TODO: Destruir o escopo!
            }
        }
        
        for(auto& id : to_remove) {
            operation_runners.erase(id);
        }

        return return_values;
    }
}