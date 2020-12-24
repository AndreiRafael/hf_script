#include "HFS_Script.hpp"
#include <list>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <regex>

#include "HFS_TokenGroup.hpp"
#include "operations/HFS_Operations.hpp"

namespace fs=std::filesystem;

namespace hfs {
    bool Script::load_from_string(const std::string_view text) {
        int line_index = 1;
        int column_index = 0;

        bool is_code = true;
        bool escaped = false;
        bool comment = false;

        Token current_token = {"", 0, 0};
        std::vector<Token> tokens = std::vector<Token>();
        for(auto c : text) {//read each char and transform them into tokens
            //simple count
            column_index++;
            if(c == '\n') {
                column_index = 0;
                line_index++;
            }

            //conditional treatment
            if(comment) {//skip until endline
                if(c == '\n') {
                    comment = false;
                }
                continue;
            }

            
            //char-based treatment
            if(c == '#' && is_code) {//found a comment
                comment = true;
                continue;
            }

            if(is_code) {
                if(c == ' ' || c == '\t' || c == '\f' || c == '\v' || c == '\n') {//whitespace or endline, add token to list
                    if(c == '\t') {
                        column_index += 3;//tab is in full 4 spaces
                    }
                    if(current_token.token.size() > 0) {
                        tokens.push_back(current_token);
                        current_token.token = "";
                    }
                }
                else if(c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')'
                     || c == ';' || c == '=' || c == ',' || c == ':')
                {
                    if(current_token.token.size() > 0) {//push both current token and bracket as a token
                        tokens.push_back(current_token);
                        current_token.token = "";
                    }
                    std::stringstream ss;
                    ss << c;

                    Token tmp = { ss.str(), line_index, column_index };
                    tokens.push_back(tmp);
                }
                else if(c == '\"') {
                    is_code = false;
                    if(current_token.token.size() == 0) {//sets line/column
                        current_token.line = line_index;
                        current_token.column = column_index;
                    }
                    current_token.token += c;
                }
                else {//just add to token
                    if(current_token.token.size() == 0) {//sets line/column
                        current_token.line = line_index;
                        current_token.column = column_index;
                    }
                    current_token.token += c;
                }
            }
            else {//literal text
                if(escaped) {
                    if(c == 'n') {
                        current_token.token += '\n';
                    }
                    else {
                        current_token.token += c;
                    }
                    escaped = false;
                }
                else {
                    if(c == '\"') {
                        current_token.token += c;
                        tokens.push_back(current_token);
                        current_token.token = "";
                        is_code = true;
                    }
                    else if(c == '\\') {
                        escaped = true;
                    }
                    else {
                        current_token.token += c;
                    }
                }
            }
        }

        int bracket_depth = 0;
        int paren_depth = 0;
        int sqrbracket_depth = 0;

        auto make_error_raw = [this] (const std::string message) -> bool {
            error_string = message;
            return false;
        };

        auto make_error = [this] (const std::string message, const Token& token) -> bool {
            std::regex r("%t");
            std::string msg = std::regex_replace(message, r, token.token);
            std::stringstream ss;
            ss << msg << " (Ln " << token.line << ", Col " << token.column << ')';
            error_string = ss.str();
            return false;
        };
        
        if(tokens.size() == 0) {
            return make_error_raw("Script had no content");
        }

        auto last_token = tokens[tokens.size() - 1];
        std::vector<std::pair<TokenGroup, Operation*>> token_groups;
        while(tokens.size() > 0) {
            TokenGroup new_group(false, bracket_depth, tokens);
            if(new_group.compile(&error_string)) {
                token_groups.push_back(std::make_pair(new_group, static_cast<Operation*>(nullptr)));

                tokens = new_group.get_remaining_tokens();
                bracket_depth = new_group.get_next_depth();
            }
            else {
                if(error_string.size() == 0) {
                    std::stringstream ss;
                    ss << "Unknown error (Ln " << tokens[0].line << ", Col " << tokens[0].column << ")";
                    error_string = ss.str();
                }
                return false;
            }
        }

        if(bracket_depth > 0) {
            return make_error("Expected '}', found end of script", last_token);
        }

        //when all tokensgroups are compiled, build operations
        for(int i = 0; i < token_groups.size(); ++i) {
            token_groups[i].second = token_groups[i].first.build_operation();
        }

        //in case some error occurs after they've been created
        auto delete_operations = [&] () {
            for(auto& pair : token_groups) {
                delete pair.second;
            }
        };

        //TODO: set operation sequencing, while loops, if/else conditions
        for(int i = 0; i < token_groups.size(); ++i) {
            auto g = token_groups[i].first;
            auto op = token_groups[i].second;
            auto info_tokens = g.get_info_tokens();
            switch (g.get_type())
            {
            case TokenGroupType::FunctionEntry:
            {
                std::vector<std::string> param_names = std::vector<std::string>(); 
                param_names.reserve(info_tokens.size() - 1);
                for(auto itr = info_tokens.begin() + 1; itr != info_tokens.end(); ++itr) {
                    param_names.push_back(itr->token);
                }
                ScriptFunctionDef new_func = { info_tokens[0].token, param_names };
                if(find_function(info_tokens[0].token, static_cast<unsigned int>(param_names.size())) == nullptr) {//good, function not yet defined                        
                    functions.push_back(std::make_pair(new_func, op));
                }
                else {
                    delete_operations();
                    return make_error("Function redefinition: '%t'", info_tokens[0]);
                }
            }
            //fall down to sequential generic
            case TokenGroupType::Set:
            case TokenGroupType::Release:
            case TokenGroupType::SubCall:
            case TokenGroupType::DeepenScope:
                if(i + 1 < token_groups.size()) {
                    auto seq = dynamic_cast<SequentialOperation*>(op);
                    seq->set_next_operation(token_groups[i + 1].second);
                }
                break;
            case TokenGroupType::FlattenScope:
                {
                    auto seq = dynamic_cast<SequentialOperation*>(op);

                    bool is_while = false;
                    for(int j = i - 1; j > 0; --j) {
                        auto other_g = token_groups[j].first;
                        auto prev_g = token_groups[j - 1].first;
                        auto prev_op = token_groups[j - 1].second;
                        if(other_g.get_depth() == g.get_depth() - 1 && prev_g.get_depth() == g.get_depth() - 1) {
                            if(other_g.get_type() == TokenGroupType::DeepenScope && prev_g.get_type() == TokenGroupType::WhileLoop) {
                                seq->set_next_operation(prev_op);
                                is_while = true;
                            }
                            break;
                        }
                    }
                    if(!is_while) {
                        for(int j = i + 1; j < token_groups.size(); ++j) {
                            auto other_g = token_groups[j].first;
                            auto other_op = token_groups[j].second;
                            if(other_g.get_depth() == g.get_depth() - 1) {//possibly a match
                                if(other_g.get_type() == TokenGroupType::ElseConditional) {
                                    continue;
                                }
                                else if(other_g.get_type() == TokenGroupType::ElseIfConditional) {
                                    j++;
                                    continue;
                                }
                                seq->set_next_operation(other_op);
                                break;
                            }
                        }
                    }
                }
                /* code */
                break;
            case TokenGroupType::ElseIfConditional:
                if(i + 1 < token_groups.size()){
                    auto branch = dynamic_cast<BranchOperation*>(op);
                    branch->set_true_operation(token_groups[i + 1].second);
                    for(int j = i + 2; j < token_groups.size(); ++j) {
                        auto other_g = token_groups[j].first;
                        auto other_op = token_groups[j].second;
                        if(other_g.get_depth() == g.get_depth()) {
                            branch->set_false_operation(other_op);
                            break;
                        }
                    }
                    for(int j = i - 1; j > 0; --j) {
                        auto other_g = token_groups[j].first;
                        auto prev_g = token_groups[j - 1].first;
                        if(other_g.get_depth() == g.get_depth() && prev_g.get_depth() == g.get_depth()) {
                            if(other_g.get_type() != TokenGroupType::DeepenScope ||
                               (prev_g.get_type() != TokenGroupType::IfConditional && prev_g.get_type() != TokenGroupType::ElseIfConditional))
                            {
                                delete_operations();
                                return make_error("'Else if' conditional found no matching conditional", g.get_first_token());
                            }
                            break;
                        }
                        else if(other_g.get_depth() < g.get_depth()) {
                            delete_operations();
                            return make_error("'Else if' conditional found no matching conditional", g.get_first_token());
                        }
                    }
                }
                break;
            case TokenGroupType::ElseConditional:  
                if(i + 1 < token_groups.size()){
                    auto seq = dynamic_cast<SequentialOperation*>(op);
                    seq->set_next_operation(token_groups[i + 1].second);                              
                    for(int j = i - 1; j > 0; --j) {
                        auto other_g = token_groups[j].first;
                        auto prev_g = token_groups[j - 1].first;
                        if(other_g.get_depth() == g.get_depth() && prev_g.get_depth() == g.get_depth()) {
                            if(other_g.get_type() != TokenGroupType::DeepenScope ||
                                (prev_g.get_type() != TokenGroupType::IfConditional && prev_g.get_type() != TokenGroupType::ElseIfConditional))
                            {
                                delete_operations();
                                return make_error("'Else' conditional found no matching conditional", g.get_first_token());
                            }
                            break;
                        }
                        else if(other_g.get_depth() < g.get_depth()) {
                            delete_operations();
                            return make_error("'Else' conditional found no matching conditional", g.get_first_token());
                        }
                    }
                }
                break;            
            case TokenGroupType::WhileLoop:
            case TokenGroupType::IfConditional:
                if(i + 1 < token_groups.size()){
                    auto branch = dynamic_cast<BranchOperation*>(op);
                    branch->set_true_operation(token_groups[i + 1].second);
                    for(int j = i + 2; j < token_groups.size(); ++j) {
                        auto other_g = token_groups[j].first;
                        auto other_op = token_groups[j].second;
                        if(other_g.get_depth() == g.get_depth()) {
                            branch->set_false_operation(other_op);
                            break;
                        }
                    }
                }
                break;
            }
        }

        compiled = true;
        return true;
    }

    bool Script::load_from_file(std::string_view path) {
        fs::path p(path);
        if(fs::exists(p) && fs::is_regular_file(p)) {
            std::string file_text;
            std::ifstream file(p);

            char c;
            while(file.get(c)) {
                if(c == '\r'){//ignore \r
                    continue;
                }
                file_text += c;
            }
            return load_from_string(file_text);
        }

        return false;
    }

    Operation* Script::find_function(const std::string_view name, const unsigned int param_count) {
        for(auto f : functions) {
            if(f.first.name.compare(name) == 0 && static_cast<unsigned int>(f.first.parameter_names.size()) == param_count) {
                return f.second;
            }
        }
        return nullptr;
    }

    std::vector<std::string> Script::get_function_parameters(const std::string_view name, const unsigned int param_count) {
        for(auto d : functions) {
            if(d.first.name.compare(name) == 0 && param_count == static_cast<unsigned int>(d.first.parameter_names.size())) {
                return d.first.parameter_names;
            }
        }
        return std::vector<std::string>();
    }

    std::string Script::get_error() const {
        return error_string;
    }

    bool Script::is_compiled() const {
        return compiled;
    }
}