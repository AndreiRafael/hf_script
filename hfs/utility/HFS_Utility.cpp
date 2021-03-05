#include "HFS_Utility.hpp"

#include <algorithm>
#include "../core/HFS_Token.hpp"

namespace hfs::utility {
    bool is_operator_symbol(const char&  c) {
        return c == '=' ||
               c == '-' ||
               c == '+' ||
               c == '*' ||
               c == '/' ||
               c == '%' ||
               c == '&' ||
               c == '|' ||
               c == '!' ||
               c == '@' ||
               c == '$' ||
               c == '>' ||
               c == '<';
    }

    bool is_special_symbol(const char& c) {
        return c == '{' ||
               c == '}' ||
               c == '[' ||
               c == ']' ||
               c == '(' ||
               c == ')' ||
               c == ',' ||
               c == ';' ||
               c == ':';
    }

    bool is_special_token(const std::string& token) {
        return token.compare("release") == 0 ||
               token.compare("true") == 0 ||
               token.compare("false") == 0 ||
               token.compare("func") == 0 ||
               token.compare("return") == 0 ||
               token.compare("null") == 0 ||
               token.compare("else") == 0 ||
               token.compare("if") == 0 ||
               token.compare("while") == 0 ||
               token.compare("delete") == 0;
    }

    bool is_raw_value (const std::string& value) {
        if(value.compare("null") == 0 ||
           value.compare("true") == 0 ||
           value.compare("false") == 0)
        {
            return true;
        }

        if(value.size() >= 2 && value[0] == '\"' && value[value.size() - 1] == '\"') {
            return true;
        }

        int dot_count = 0;
        int num_count = 0;
        int hifen_count = 0;
        for (auto c : value) {
            if(c == '.') {
                dot_count++;
            }
            else if (c >= 48 && c <= 57) {
                num_count++;
            }
            else if(num_count == 0 && dot_count == 0 && c == '-') {
                hifen_count++;
            }
        }
        return dot_count <= 1 && hifen_count <= 1 && num_count > 0 && num_count + dot_count + hifen_count == value.size();
    }

    //validates wheter a string may be used as a variable or function name
    bool validate_name(const std::string& name) {
        if(is_special_token(name)) {
            return false;
        }

        if(name.size() > 0) {
            auto itr = name.begin();
            if(*itr == '_' || (*itr >= 65 && *itr <= 90) || (*itr >= 97 && *itr <= 122)) {//first char cannot be a number
                return std::all_of(++itr, name.end(), [] (char c) { 
                                                                      return c == '_' ||
                                                                            (c >= 65 && c <= 90) ||
                                                                            (c >= 97 && c <= 122) ||
                                                                            (c >= 48 && c <= 57);
                                                                  }
                );
            }
        }
        return false;
    }

    core::TokenType determine_token_type(const std::string& token) {
        if(utility::validate_name(token)) {
            return core::TokenType::Name;
        }
        else if(utility::is_special_token(token)) {
            if(utility::is_raw_value(token)) {
                return core::TokenType::RawValue;
            }
            return core::TokenType::Special;
        }
        else if(token.size() > 0 && std::all_of(token.begin(), token.end(), [] (const char& c) { return is_operator_symbol(c);} )) {
            if(token.size() == 1 && token[0] == '=') {
                return core::TokenType::SetOperator;
            }
            return core::TokenType::Operator;
        }
        else if(token.size() == 1 && is_special_symbol(token[0])) {
            return core::TokenType::Symbol;
        }

        return core::TokenType::Invalid;
    }
}