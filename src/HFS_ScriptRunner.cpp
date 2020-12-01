#include "HFS_ScriptRunner.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

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

    bool ScriptRunner::load_script(std::string_view file_path, std::string* const error_string) {
        fs::path path(file_path.data());

        auto set_error = [error_string, path] (const std::string value, const int line_num) {
            if(error_string != nullptr) {
                if(line_num >= 0) {
                    if(fs::is_regular_file(path)) {
                        *error_string = path.filename().string();
                    }
                    *error_string += std::string("(Line ") + std::to_string(line_num) + std::string("): ");
                }
                *error_string += value;
                if(value.size() > 0) {
                    std::string(".");  
                }             
            }
        };

        if(fs::is_regular_file(path)) {//only reads regular files
            std::ifstream file_stream(path);
            std::list<std::string> full_file;
            
            bool reading_code = true;
            bool escaped = false;

            //first step, line reconstruction
            std::size_t curr_index = 0;
            std::list<std::pair<std::size_t, std::size_t>> text_intervals;
            while(file_stream) {
                std::string line;
                std::getline(file_stream, line);
                line = std::regex_replace(line, std::regex("\\s"), " ");//replaces tabs and other whitespaces

                std::list<std::string> code_text_line;
                code_text_line.push_back("");//alternating code text lines
                if(!reading_code) {
                    code_text_line.push_back("");
                }
                for(const auto& c : line) {
                    bool reset = escaped;
                    if(reading_code) {
                        if(c == '\"') {//started text
                            reading_code = false;
                            code_text_line.push_back("");//starts a text section
                        }
                        if(c == '#') {//gets out of for bc of comment
                            break;
                        }
                    }
                    else {//reading text
                        if(c == '\"' && !escaped) {
                            reading_code = true;
                            code_text_line.back().push_back(c);
                            code_text_line.push_back("");
                            continue;
                        }
                        if(c == '\\') {
                            escaped = true;
                        }
                    }
                    code_text_line.back().push_back(c);
                    if(reset) {
                        escaped = false;
                    }
                }

                full_file.push_back("");
                bool is_code = false;
                for(auto& str : code_text_line) {
                    is_code = !is_code;
                    if(is_code) {
                        str = std::regex_replace(str, std::regex("(?:\\s|\\r|\\n)*(,|\\{|\\}|\\(|\\)|=|;)(?:\\s|\\r|\\n)*"), "$1");
                        str = std::regex_replace(str, std::regex("\\s+"), " ");
                        if(full_file.back().size() == 0 && str.size() > 0 && str[0] == ' ') {
                            str.erase(0, 1);
                        }
                    }
                    else {
                        text_intervals.push_back(std::make_pair(curr_index, curr_index + str.size()));
                    }
                    full_file.back().append(str);
                    curr_index += str.size();
                }

                if(full_file.back().size() > 0) {
                    if(!is_code) {
                        curr_index++;
                        full_file.back().push_back(' ');
                        text_intervals.back().second++;
                    }
                    else {
                        switch (full_file.back()[full_file.back().size() - 1])
                        {
                        case '{':
                        case '}':
                        case '(':
                        case ')':
                        case ';':
                        case '=':
                        case ',':
                        case ' ':
                            break;
                        default:// Adds a space behind a sole character at the end of a line
                            curr_index++;
                            full_file.back().push_back(' ');
                            break;
                        }
                    }
                }
            }

            // TODO: BEWARE! This next step is full of Go Horse Style code

            //second step: command separation + code validation         
            int bracket_depth = 0;
            int paren_depth = 0;
            int paren_max = 0;
            int line_number = 0;
            bool is_set = false;//is set operation
            curr_index = 0;

            std::list<std::pair<int, std::string>> commands;//pair of depth/text
            std::string current_cmd;
            std::vector<bool> cmd_is_text;
            std::vector<int> cmd_line;
            for(const auto& line : full_file) {
                line_number++;
                
                for(const auto& c : line) {
                    auto is_text = [text_intervals] (const std::size_t index_to_check) {
                        return std::any_of(text_intervals.begin(), text_intervals.end(), [index_to_check] (std::pair<std::size_t, std::size_t> pair) {
                            return index_to_check >= pair.first && index_to_check < pair.second;
                        });
                    };
                    auto is_cond_or_loop = [](const std::string token) {
                        return token.compare("if") == 0 ||
                               token.compare("else") == 0 ||
                               token.compare("else if") == 0 ||
                               token.compare("while") == 0;
                    };
                    //retuns true if token is a keyword
                    auto is_keyword = [is_cond_or_loop](const std::string token) {
                        return token.compare("return") == 0 ||
                               token.compare("wait") == 0 ||
                               token.compare("null") == 0 ||
                               token.compare("true") == 0 ||
                               token.compare("false") == 0 ||
                               token.compare("func") == 0 ||
                               is_cond_or_loop(token);
                    };

                    //validates current command as an operation defined with a bracket (func defs, ifs, while)
                    auto validade_bracket_call = [&] () -> bool {
                        //being here we are already sure this has bracket/paren validated
                        if(current_cmd.size() == 0) {
                            set_error("Unexpected \'{\'", line_number);
                            return false;
                        }

                        if(bracket_depth == 0) {//must be a function
                            if(current_cmd.substr(0, 5).compare("func ") != 0) {
                                set_error(std::string("Expected function definition"), line_number);
                                return false;
                            }

                            auto space_count = std::count(current_cmd.begin(), current_cmd.end(), ' ');
                            if(space_count != 1) {//must have exactly one space at func defs
                                set_error(std::string("Invalid definition, too many spaces: ") + current_cmd, line_number);
                                return false;
                            }
                            if(paren_max != 1) {
                                set_error(std::string("Invalid function definition: ") + current_cmd, line_number);
                                return false;
                            }

                            //function confirmed, start reading func name
                            std::string mini_token = "";
                            std::string func_name = "";
                            int i = 5;
                            for(; i < current_cmd.size(); ++i) {
                                const auto& ch = current_cmd[i]; 
                                if(ch == '(') {
                                    func_name = mini_token;
                                    mini_token = "";
                                    break;
                                }
                                mini_token += ch;
                            }

                            if(func_name.size() == 0) {
                                set_error(std::string("Expected function name: ") + current_cmd, line_number);
                                return false;
                            }
                            else if(!std::regex_match(func_name, std::regex("[a-zA-Z]\\w*")) || is_keyword(func_name)) {
                                set_error(std::string("Invalid function name: ") + func_name, line_number);
                                return false;
                            }

                            int param_count = 0;
                            ++i;
                            for(;i < current_cmd.size(); ++i) {
                                const auto& ch = current_cmd[i];

                                if(ch == ')') {
                                    if(mini_token.size() == 0) {
                                        if(param_count > 0) {
                                            set_error(std::string("Expected parameter definition at function ") + func_name, line_number);
                                            return false;
                                        }
                                    }
                                    else if (is_keyword(mini_token)){
                                        set_error(std::string("Invalid parameter name: ") + mini_token, line_number);
                                        return false;
                                    }
                                    continue;
                                }
                                if(ch == ',') {
                                    if(mini_token.size() == 0) {
                                        set_error(std::string("Unexected \',\'"), line_number);
                                        return false;
                                    }
                                    if(!std::regex_match(mini_token, std::regex("[a-zA-Z]\\w*")) || is_keyword(mini_token)) {
                                        set_error(std::string("Invalid parameter name: ") + mini_token, line_number);
                                        return false;
                                    }
                                    mini_token = "";
                                    param_count++;
                                    continue;
                                }

                                if(param_count == 0) {
                                    param_count = 1;
                                }
                                mini_token += ch;
                            }
                        }
                        else {//must be if/else if/while
                            int i = 0;
                            std::string mini_token = "";
                            for(;i < current_cmd.size(); ++i) {
                                const auto& ch = current_cmd[i];
                                if(ch == '(') {// TODO: checar por texto, e invalidar direto
                                    break;
                                }
                                mini_token += ch;
                            }

                            if(!is_cond_or_loop(mini_token)) {// TODO: Lembarar de ajeitar esse line number
                                set_error(std::string("Invalid operation: ") + mini_token, line_number);
                                return false;
                            }
                        }
                        return true;
                    };
                    

                    if(is_text(curr_index)) {
                        if(!is_set && paren_depth == 0) {
                            set_error(std::string("string definition outside operation"), line_number);
                            return false;
                        }
                        current_cmd += c;
                    }
                    else {
                        switch (c)
                        {
                        case '{':
                            if(!validade_bracket_call()) {
                                return false;
                            }
                            commands.push_back(std::make_pair(bracket_depth, current_cmd));
                            current_cmd = "";
                            paren_max = 0;//resets parenthesis count
                            bracket_depth++;
                            break;
                        case '}':
                            if(current_cmd.size() > 0) {
                                set_error("Expected \';\' before \'}\'", line_number);
                                return false;
                            }
                            bracket_depth--;
                            if(bracket_depth < 0) {
                                set_error("Unexpected \'}\'", line_number);
                                return false;
                            }
                            break;
                        case '('://open parenthesis
                            if(current_cmd.size() == 0) {
                                set_error("Unexpected \'(\'", line_number);
                                return false;
                            }
                            current_cmd += c;
                            if(paren_depth == 0 && paren_max > 0) {
                                set_error("Unexpected \'(\'", line_number);
                                return false;
                            }
                            paren_depth++;
                            paren_max = std::max(paren_max, paren_depth);
                            break;
                        case ')'://close parenthesis
                            current_cmd += c;
                            paren_depth--;
                            if(paren_depth < 0) {
                                set_error("Unexpected \')\'", line_number);
                                return false;
                            }
                            break;
                        case ';'://ends a command
                            // TODO: validade ; command
                            commands.push_back(std::make_pair(bracket_depth, current_cmd));
                            current_cmd = "";
                            paren_max = 0;//resets parenthesis count
                            break;
                        case '=':
                            
                            break;
                        case '+':
                        case '-':
                        case '/':
                        case '*':
                            set_error(std::string("Invald symbol: \'") + c  + '\'', line_number);
                            return false;
                            break;
                        default://letras e numeros
                            if(paren_max > 0 && paren_depth == 0) {//missing ';'
                                set_error("Expected ';'", line_number);
                                return false;
                            }
                            current_cmd += c;
                            break;
                        }
                    }

                    curr_index++;
                }
            }

            if(bracket_depth > 0) {
                set_error("Missing \'}\' before end of file", line_number);
                return false;
            }

            for(int i = 0; i < commands.size(); ++i) {
               
            }

            set_error("", -1);
            return true;
        }
        set_error("File does not exist!", -1);
        return false;
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