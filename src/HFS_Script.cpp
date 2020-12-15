#include "HFS_Script.hpp"
#include <list>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace fs=std::filesystem;

namespace hfs {
    bool Script::validate_text(const std::string text) {
        return false;
    }

    bool Script::validate_call(const std::string text) {
        return false;
    }

    bool Script::load_from_string(const std::string_view text) {

        int line_index = 0;
        int char_index = 0;

        bool is_code = true;
        bool escaped = false;
        bool comment = false;

        std::string current_token = "";
        std::vector<std::string> tokens = std::vector<std::string>();
        for(auto c : text) {//read each char and transform them into tokens
            //simple count
            char_index++;
            if(c == '\n') {
                char_index = 0;
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
                    if(current_token.size() > 0) {
                        tokens.push_back(current_token);
                        current_token = "";
                    }
                }
                else if(c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')'
                     || c == ';' || c == '=' || c == ',' || c == ':')
                {
                    if(current_token.size() > 0) {//push both current token and bracket as a token
                        tokens.push_back(current_token);
                        current_token = "";
                    }
                    std::stringstream ss;
                    ss << c;
                    tokens.push_back(ss.str());
                }
                else if(c == '\"') {
                    is_code = false;
                    current_token += c;
                }
                else {//just add to token
                    current_token += c;
                }
            }
            else {//literal text
                if(escaped) {
                    current_token += c;
                    escaped = false;
                }
                else {
                    if(c == '\"') {
                        current_token += c;
                        tokens.push_back(current_token);
                        current_token = "";
                        is_code = true;
                    }
                    else if(c == '\\') {
                        escaped = true;
                    }
                    else {
                        current_token += c;
                    }
                }
            }
        }

        int bracket_depth = 0;
        int paren_depth = 0;
        int sqrbracket_depth = 0;

        std::string cmd_ender = "";
        std::vector<std::string> cmd_tokens;
        for(auto token : tokens) {
            
        }

        return false;
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
}