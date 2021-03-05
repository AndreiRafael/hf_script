#include "HFS_ScriptCompiler.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include "./utility/HFS_Utility.hpp"

namespace fs = std::filesystem;

namespace hfs {
    bool ScriptCompiler::is_token_separator(const char& c) {
        return c == ' ' ||
               c == '\t' ||
               c == '\f' ||
               c == '\v' ||
               c == '\n';
    }

    bool ScriptCompiler::validate_operator(std::string const& symbol) {
        if(symbol.size() == 0 || symbol.compare("=") == 0) {
            return false;
        }

        return std::all_of(symbol.begin(), symbol.end(), [] (char const& c) { return utility::is_operator_symbol(c); });
    }

    bool ScriptCompiler::get_operator(const std::string symbol, const OperatorType type, Operator* op) const{
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
            *op = { "", "", OperatorType::None };
        }
        return false;
    }

    void ScriptCompiler::push_log(const LogType type, const std::string text, core::Token const& token) {
        std::string msg = std::regex_replace(text, std::regex("%t"), token.token);
        std::stringstream ss;
        ss << msg << " (Ln " << token.line << ", Col " << token.column << ')';
        LogData new_log = { type, ss.str() };
        logs.push_back(new_log);
    }

    bool ScriptCompiler::define_operator(const std::string symbol, const std::string function, const OperatorType type) {
        if(type == OperatorType::None || type == OperatorType::Any || !validate_operator(symbol) || get_operator(symbol, type, nullptr)) {
            return false;// TODO: push error logs for each error type
        }

        Operator o = { symbol, function, type };
        operators.push_back(o);
        return true;
    }

    bool ScriptCompiler::undefine_operator(const std::string symbol, const OperatorType type) {
        auto comp = [&] (Operator const& op) { return op.type == type && op.symbol.compare(symbol) == 0; };
        auto itr = std::find_if(operators.begin(), operators.end(), comp);
        if(itr != operators.end()) {
            operators.erase(std::find_if(operators.begin(), operators.end(), comp)); 
            return true;
        }
        return false;
    }

    std::vector<core::Token> ScriptCompiler::tokenize_from_file(std::string path) {
        fs::path p(path);
        if(fs::exists(p) && fs::is_regular_file(p)) {
            std::string file_text;
            std::ifstream file(p);

            char c;
            while(file.get(c)) {
                file_text += c;
            }
            return tokenize_from_text(file_text);
        }

        return std::vector<core::Token>();
    }

    std::vector<core::Token> ScriptCompiler::tokenize_from_text(std::string text) {
        std::remove_if(text.begin(), text.end(), [] (char const& ch) { return ch == '\r'; });

        auto itr = text.begin();
        unsigned int line = 0;
        unsigned int column = 0;
        auto step_itr= [&] (char* c) -> bool {
            if(itr != text.end() && ++itr != text.end()) {
                ++column;
                switch (*itr)
                {
                case '\n':
                    ++line;
                    column = 0;
                    break;
                case '\t':
                    column += 3;//tabs account for 4 spaces
                    break;
                }

                if(c != nullptr) {
                    *c = *itr; 
                }
                return true;
            }
            return false;
        };

        auto token_vector = std::vector<core::Token>();

        core::Token current_token;
        auto reset_token = [&current_token, &line, &column] () {
            current_token = { core::TokenType::Invalid, "", line, column };
        };
        reset_token();

        auto push_token = [&reset_token, &current_token, &token_vector] () {
            if(current_token.token.size() > 0) {
                current_token.type = utility::determine_token_type(current_token.token);
                token_vector.push_back(current_token);
            }
            reset_token();
        };

        auto char_to_token = [&current_token, &reset_token] (const char c) {
            if(current_token.token.size() == 0) {
                reset_token();
            }
            current_token.token += c;
        };

        bool is_code = true;

        char c;//char to check
        while(step_itr(&c)) {
            if(is_code) {
                if(c == '#') {//found comment, will skip until \n
                    push_token();
                    while (step_itr(&c) && c != '\n'){}
                    continue;
                }
                else if (c == '\"') {//start text
                    is_code = false;
                    push_token();
                    char_to_token(c);
                    continue;
                }
                else if (utility::is_operator_symbol(c)) {
                    push_token();
                    std::string operator_str = "";
                    operator_str += c;
                    while(step_itr(&c) && utility::is_operator_symbol(c)) {
                        operator_str += c;
                    }
                    //if next token is likely a number, consumes last operator if it is a minus sign
                    bool minus_cut = (c == 46 || (c >= 48 && c <=57)) && operator_str[operator_str.length() - 1] == '-';
                    auto breakdown_operator = [this, &operator_str] () -> bool {
                        if(operator_str.compare("=") != 0 && get_operator(operator_str, OperatorType::Any, nullptr)) {
                            return false;
                        }
                        if(operator_str[0] == '=') {//separates into two operators
                            operator_str = operator_str.substr(0, operator_str.length() - 1);
                            return true;
                        }
                        return false;
                    };
                    if(breakdown_operator()) {
                        char_to_token('=');
                        push_token();
                    }
                    for(int i = 0; i < operator_str.length() - (minus_cut ? 1 : 0); ++i) {
                        char_to_token(operator_str[i]);
                    }
                    push_token();
                    if(minus_cut) {
                        char_to_token('-');
                    }
                    char_to_token(c);
                }
                else if (c == ' ' || c == '\n') {
                    push_token();
                }
                else if (utility::is_special_symbol(c)) {
                    push_token();
                    char_to_token(c);//pushes the symbol as its own token
                    push_token();
                }
                else if (c == ' ') {
                    push_token();
                }
            }
            else {//text, find text-ending conditions, or just copy text to token
                if(c == '\"') {//end text
                    char_to_token(c);
                    push_token();
                    is_code = true;
                }
                else if (c == '\\') {//escaped char
                    if(step_itr(&c)) {
                        if(c == 'n') {
                            char_to_token('\n');
                        }
                        else {
                            char_to_token(c);
                        }
                    }
                }
            }
        }

        if(!is_code) {
            push_log(LogType::Error, "Text ended unexpectedly!", current_token);
        }
        if(token_vector.size() == 0) {
            push_log(LogType::Error, "No tokens found!", current_token);
        } 

        return token_vector;
    }
}