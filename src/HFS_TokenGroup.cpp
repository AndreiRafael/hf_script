#include "HFS_TokenGroup.hpp"
#include <sstream>
#include <algorithm>
#include <regex>

#include "operations/HFS_Operations.hpp"

namespace hfs{
    TokenGroup::TokenGroup(const bool sub_group, const int depth, const std::vector<Token> tokens) {
        this->sub_group = sub_group;//sub group allows return of non sequential operation
        this->depth = depth;
        this->tokens = tokens;
    }

    bool TokenGroup::compile(std::string* error_string) {
        auto clear_error = [error_string] () {
            if(error_string != nullptr) {
                *error_string = "";
            }
        };

        auto make_error = [this, error_string] (const std::string message, const Token& token) -> bool {
            if(error_string != nullptr) {
                std::regex r("%t");
                std::string msg = std::regex_replace(message, r, token.token);
                std::stringstream ss;
                ss << msg << " (Ln " << token.line << ", Col " << token.column << ')';
                *error_string = ss.str();
            }
            operation = nullptr;
            return false;
        };

        auto make_error_unexpected = [make_error] (const Token& token) -> bool {
            return make_error("Unexpected token: '%t'", token);
        };

        //validates wheter a string may be used as a variable or function name
        auto validate_name = [] (std::string name) -> bool {
            if(name.compare("release") == 0 ||
               name.compare("true") == 0 ||
               name.compare("false") == 0 ||
               name.compare("func") == 0 ||
               name.compare("return") == 0 ||
               name.compare("null") == 0 ||
               name.compare("else") == 0 ||
               name.compare("if") == 0 ||
               name.compare("while") == 0)
            {
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
        };

        auto is_raw = [] (const std::string value) {
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
        };

        //to be  used right after a '('. itr must be an iterator of 'tokens'. itr will be after the ')' if everithing went right
        //if it returns true, 'parameters' will contain valid operation to be used as parameters
        auto get_function_parameters = [this, make_error, make_error_unexpected, error_string] (std::vector<Token>::iterator& itr, std::vector<Operation*>* parameters) -> bool {
            int paren_depth = 1;

            std::vector<Token> current_param = std::vector<Token>();
            while(itr != tokens.end() && paren_depth > 0) {
                bool gen = false;
                if(itr->token.compare(",") == 0) {
                    if(paren_depth == 1) {//ended a param
                        if(current_param.size() == 0) {//no data
                            return make_error("Expected parameter before comma", *itr);
                        }
                        gen = true;
                    }
                    else {
                        current_param.push_back(*itr);
                    }
                }
                else {
                    if(itr->token.compare(")") == 0) {
                        if(--paren_depth == 0) {
                            gen = current_param.size() > 0;
                        }
                    }
                    else if(itr->token.compare("(") == 0) {
                        paren_depth++;
                    }

                    if(paren_depth > 0) {
                        current_param.push_back(*itr);
                    }
                }

                itr++;
                if(gen) {
                    TokenGroup param_group(true, this->get_depth(), current_param);
                    if(param_group.compile(error_string)) {
                        parameters->push_back(param_group.get_operation());
                    }
                    else {
                        return false;
                    }
                    current_param.clear();
                }
            }

            if(current_param.size() > 0) {
                return make_error_unexpected(current_param[0]);
            }
            return true;
        };

        if(depth == 0) {//must be func def
            for(int i = 0; i < tokens.size(); i++) {
            auto token = tokens[i];
                switch (i)
                {
                case 0://must be 'func'
                    if(token.token.compare("func") != 0) {
                        return make_error("Expected 'func', found '%t'", token);
                    }
                    type = TokenGroupType::FunctionEntry;
                    break;
                case 1://must be a valid function name
                    if(!validate_name(token.token)) {
                        return make_error("Invalid function name: '%t'", token);
                    }
                    info_tokens.push_back(token);
                    break;
                case 2://must be '('
                    if(token.token.compare("(") != 0) {
                        return make_error("Expected '(', found '%t'", token);
                    }
                    break;                
                default://param names, ',' or ')'
                    if(token.token.compare(")") == 0) {//must be even if no params, odd otherwise
                        if(info_tokens.size() == 1) {//no params, must be odd
                            if((i % 2) == 0) {//not even, error
                                return make_error_unexpected(token);
                            }
                        }
                        else {//has params
                            if((i % 2) != 0) {//even, error
                                return make_error_unexpected(token);
                            }
                        }
                        //next token must be '{'
                        if(i + 1 >= tokens.size()) {
                            return make_error("Expected '{' after function definition", token);
                        }
                        else if(tokens[i + 1].token.compare("{") != 0) {
                            return make_error("Expected '{' after function definition, found '%t'", tokens[i + 1]);
                        }
                        else {//function definition success
                            clear_error();
                            compiled = true;
                            remaining_tokens = std::vector<Token>(tokens.begin() + i + 2, tokens.end());
                            next_depth = 1;
                            operation = new DeepenScopeOperation();
                            return true;
                        }
                    }
                    else if(token.token.compare(",") == 0) {
                        if((i % 2) != 0) {//not even index, error
                            return make_error_unexpected(token);
                        }
                    }
                    else {
                        if((i % 2) == 0) {//even index, error
                            return make_error_unexpected(token);
                        }
                        else if(!validate_name(token.token)) {//invalid name
                            return make_error("Invalid parameter name: '%t'", token);
                        }
                        else {//valid parameter
                            info_tokens.push_back(token);
                        }
                    }
                    break;
                }     
            }
        }//depth == 0
        else if(tokens.size() > 0) {//depth != 0
            first_token = tokens[0];
            if(!sub_group) {//may be return/release/if/else if/else
                //TODO: pure function calls can also be here (via sub calls)
                if(first_token.token.compare("if") == 0 ||
                   first_token.token.compare("else") == 0 ||
                   first_token.token.compare("while") == 0)
                {//IF, IF/ELSE or ELSE or WHILE
                    auto itr = tokens.begin() + 1;
                    if(first_token.token.compare("else") == 0) {
                        if(itr == tokens.end()) {
                            return make_error("Unexpected end of file", *--itr);
                        }
                        else if(itr->token.compare("if") == 0) {//is if else
                            type = TokenGroupType::ElseIfConditional;
                            itr++;
                        }
                        else {//simply else
                            if(itr->token.compare("{") != 0) {
                                return make_error("Expected '{', found '%t'", *itr);
                            }
                            type = TokenGroupType::ElseConditional;

                            clear_error();
                            compiled = true;
                            remaining_tokens = std::vector<Token>(++itr, tokens.end());
                            next_depth = depth + 1;
                            operation = new DeepenScopeOperation();
                            return true;
                        }
                    }
                    else if(first_token.token.compare("while") == 0) {
                        type = TokenGroupType::WhileLoop;
                    }
                    else {//if
                        type = TokenGroupType::IfConditional;
                    }

                    if(itr == tokens.end()) {
                        return make_error("Expected '(', found end of file", first_token);
                    }
                    else if(itr->token.compare("(") != 0) {
                        return make_error("Expected '(', found '%t'", *itr);
                    }
                    else {
                        int paren_depth = 1;

                        std::vector<Token> sub_tokens = std::vector<Token>();
                        while(++itr != tokens.end()) {
                            if(itr->token.compare(",") == 0) {
                                if(paren_depth == 1) {//illegal char
                                    sub_tokens.push_back(*itr);
                                    TokenGroup condition(true, depth, sub_tokens);
                                    if(!condition.compile(error_string)) {//try to compile token in search of relevant error
                                        return false;
                                    }
                                    //TODO: free operation
                                    return make_error_unexpected(*itr);
                                }
                            }
                            else if(itr->token.compare("(") == 0) {
                                paren_depth++;
                            }
                            else if(itr->token.compare(")") == 0) {
                                if(--paren_depth == 0) {//completed if arguments
                                    TokenGroup condition(true, depth, sub_tokens);
                                    if(!condition.compile(error_string)) {
                                        return false;
                                    }

                                    if(++itr == tokens.end()) {
                                        return make_error("Expected '{', but reached end of file '%t' ", *(itr - 1));
                                    }
                                    else if(itr->token.compare("{") != 0) {
                                        return make_error("Expected '{', found '%t' ", *itr);
                                    }

                                    clear_error();
                                    compiled = true;
                                    remaining_tokens = std::vector<Token>(itr, tokens.end());
                                    next_depth = depth;
                                    operation = new BranchOperation(condition.get_operation());
                                    return true;
                                }
                            }
                            sub_tokens.push_back(*itr);
                        }
                    }
                }//If
                else if(first_token.token.compare("return") == 0) {//RETURN
                    type = TokenGroupType::Return;

                    auto itr = tokens.begin();
                    std::vector<Token> ret_tokens = std::vector<Token>();
                    while(++itr != tokens.end()) {
                        if(itr->token.compare(";") == 0) {
                            if(ret_tokens.size() == 0) {
                                clear_error();
                                compiled = true;
                                remaining_tokens = std::vector<Token>(++itr, tokens.end());
                                next_depth = depth;
                                operation = new RawValueOperation("null");
                                return true;
                            }
                            else {
                                TokenGroup ret_group(true, depth, ret_tokens);
                                if(ret_group.compile(error_string)) {
                                    clear_error();
                                    compiled = true;
                                    remaining_tokens = std::vector<Token>(++itr, tokens.end());
                                    next_depth = depth;
                                    operation = ret_group.get_operation();
                                    return true;
                                }
                                else {
                                    return false;
                                }
                            }
                        }
                    }
                }
                else if(first_token.token.compare("release") == 0) {//RETURN
                    type = TokenGroupType::Release;
                    
                    auto itr = tokens.begin();
                    if(++itr == tokens.end()) {
                        return make_error("Expected ';', reached end of file", *--itr);
                    }
                    else if(itr->token.compare(";") != 0) {
                        return make_error("Expected ';', found '%t'", *itr); 
                    }

                    clear_error();
                    compiled = true;
                    remaining_tokens = std::vector<Token>(++itr, tokens.end());
                    next_depth = depth;
                    operation = new ReleaseOperation();
                    return true;
                }
                else if(first_token.token.compare("{") == 0) {//deepen
                    type = TokenGroupType::DeepenScope;

                    clear_error();
                    compiled = true;// TODO: this does not guarantee compilation
                    remaining_tokens = std::vector<Token>(tokens.begin() + 1, tokens.end());
                    next_depth = depth + 1;

                    operation = new DeepenScopeOperation();
                    return true;
                }
                else if(first_token.token.compare("}") == 0) {//flatten
                    operation = new FlattenScopeOperation();
                    type = depth == 1 ? TokenGroupType::FunctionEnd : TokenGroupType::FlattenScope;

                    remaining_tokens = std::vector<Token>(tokens.begin() + 1, tokens.end());
                    clear_error();
                    compiled = true;// TODO: this does not guarantee compilation
                    next_depth = depth - 1;

                    return true;
                }
                else if(validate_name(first_token.token)) {//valid name, set operation of fcall
                    if(tokens.size() > 1 && tokens[1].token.compare("(") == 0) {//function call
                        type = TokenGroupType::SubCall;
                        auto itr = tokens.begin() + 2;
                        std::vector<Operation*> parameters;
                        bool param_res = get_function_parameters(itr, &parameters);
                        if(param_res && itr != tokens.end() && itr->token.compare(";") == 0){//valid fcall
                            clear_error();
                            compiled = true;// TODO: this does not guarantee compilation
                            remaining_tokens = std::vector<Token>(++itr, tokens.end());
                            next_depth = depth;

                            operation = new SubCallOperation(new FunctionCallOperation(tokens[0].token, parameters));
                            return true;
                        }
                        else {
                            //TODO: delete operations
                            if(!param_res) {
                                return false;
                            }
                            else if(itr == tokens.end()) {
                                return make_error("Expected ';', reached end of file", *--itr);
                            }
                            else if(itr->token.compare(";") != 0) {
                                return make_error("Expected ';', found '%t'", *itr); 
                            }
                            return false;
                        }
                    }
                    else {//has to be set operation
                        type = TokenGroupType::Set;
                        auto itr = tokens.begin();
                        bool eq_found = false;
                        std::vector<Token> pre_eq_tokens = std::vector<Token>();
                        pre_eq_tokens.push_back(*itr);
                        std::vector<Token> post_eq_tokens = std::vector<Token>();
                        while(++itr != tokens.end()) {//read until ';', look for '='                            
                            if(itr->token.compare("=") == 0) {
                                if(eq_found) {
                                    return make_error_unexpected(*itr);
                                }
                                if(pre_eq_tokens.size() == 0) {
                                    return make_error("Expected value before '='", *itr);
                                }
                                eq_found = true;
                            }
                            else if(itr->token.compare(";") == 0) {
                                if(eq_found) {
                                    if(post_eq_tokens.size() == 0) {
                                        return make_error("Expected value after '=', found ';'", *itr);
                                    }
                                    TokenGroup pre_group(true, depth, pre_eq_tokens);
                                    TokenGroup post_group(true, depth, post_eq_tokens);
                                    if(pre_group.compile(error_string) && post_group.compile(error_string)) {
                                        if(pre_group.type != TokenGroupType::VariableRetrieval) {
                                            return make_error("Invalid value before '=', must be a variable", pre_group.first_token);
                                        }
                                        clear_error();
                                        compiled = true;
                                        remaining_tokens = std::vector<Token>(++itr, tokens.end());
                                        next_depth = depth;

                                        operation = new SetOperation(dynamic_cast<VariableRetrievalOperation*>(pre_group.get_operation()), post_group.get_operation());
                                        return true;
                                    }
                                    else {
                                        //TODO: delete unused compiled stuff?
                                        return false;
                                    }
                                    break;
                                }
                                else {
                                    return make_error_unexpected(*itr);
                                }
                            }
                            else {
                                (eq_found ? post_eq_tokens : pre_eq_tokens).push_back(*itr);
                            }
                        }
                    }
                }
                else {
                    if(first_token.token.compare("func") == 0) {
                        return make_error("Invalid token: '%t', you may be missing a '}'", first_token);
                    }
                    return make_error("Invalid token: '%t'", first_token);
                }
            }
            else {//subgroup
                if(is_raw(first_token.token)) {//must be the only token
                    if(tokens.size() > 1) {
                        return make_error_unexpected(tokens[1]);
                    }

                    type = TokenGroupType::RawValue;
                    clear_error();
                    compiled = true;
                    remaining_tokens = std::vector<Token>();
                    next_depth = depth;
                    operation = new RawValueOperation(first_token.token);
                    return true;
                }
                else if(validate_name(first_token.token)) {//must be variable or function
                    if(tokens.size() == 1 || tokens[1].token.compare("[") == 0) {//must be a variable retrieval
                        type = TokenGroupType::VariableRetrieval;
                        clear_error();
                        compiled = true;// TODO: this does not guarantee compilation
                        remaining_tokens = std::vector<Token>();
                        next_depth = depth;
                        
                        auto retrieval_operation = new VariableRetrievalOperation(first_token.token);
                        auto itr = tokens.begin();

                        int sqr_bracket_depth = 0;
                        std::vector<Token> key_tokens = std::vector<Token>();
                        while(++itr != tokens.end()) {
                            if(sqr_bracket_depth == 0) {//must be '['
                                if(itr->token.compare("[") != 0) {
                                    return make_error("Expected '[', found '%t' ", *itr);// TODO: cleanup retrieval operation
                                }
                                ++sqr_bracket_depth;
                            }
                            else {//can be anything, will be evaluated later
                                if(itr->token.compare("[") == 0) {
                                    ++sqr_bracket_depth;
                                }
                                else if(itr->token.compare("]") == 0) {
                                    if(--sqr_bracket_depth == 0) {//finished token
                                        if(key_tokens.size() == 0) {
                                            return make_error("Expected value between '[]'", tokens[1]);// TODO: cleanup retrieval operation
                                        }

                                        TokenGroup in_bracket_tokens(true, depth, key_tokens);
                                        if(in_bracket_tokens.compile(error_string)) {
                                            retrieval_operation->add_dictionary_key(in_bracket_tokens.get_operation());
                                        }
                                        else {
                                            return false;
                                        }

                                        key_tokens.clear();
                                    }
                                }
                                
                                if(sqr_bracket_depth != 0) {
                                    key_tokens.push_back(*itr);
                                }
                            }
                        }

                        if(sqr_bracket_depth != 0) {
                            return make_error("Missing ']' after '%t'", *(tokens.end() - 1));
                        }
                        
                        operation = retrieval_operation;
                        return true;
                    }
                    else if(tokens[1].token.compare("(") == 0) {//func call
                        auto itr = tokens.begin() + 2;
                        std::vector<Operation*> parameters;
                        if(get_function_parameters(itr, &parameters)){
                            if(itr != tokens.end()) {
                                return make_error_unexpected(*itr);//clear param ops
                            }
                            clear_error();
                            compiled = true;
                            remaining_tokens = std::vector<Token>();
                            next_depth = depth;

                            operation = new FunctionCallOperation(tokens[0].token, parameters);
                            return true;
                        }
                        else {
                            //TODO: delete operations
                            return false;
                        }
                    }
                    else {
                        return make_error_unexpected(tokens[1]);
                    }
                }
                else if(first_token.token.compare("[") == 0) {//dictionary creation
                    //TODO:
                    type = TokenGroupType::DictionaryCreation;
                    auto itr = tokens.begin();
                    int sqr_bracket_depth = 1;

                    bool separator_found = false;
                    int fallback_index = 0;
                    std::vector<Token> sub_tokens_1 = std::vector<Token>();//tokens b4 separator, if there is one
                    std::vector<Token> sub_tokens_2 = std::vector<Token>();

                    std::vector<std::pair<TokenGroup, TokenGroup>> entries = std::vector<std::pair<TokenGroup, TokenGroup>>();
                    while(++itr != tokens.end()) {
                        if((itr->token.compare(",") == 0 && sqr_bracket_depth == 1) ||
                           (itr->token.compare("]") == 0 && --sqr_bracket_depth == 0)) {//finish entry
                            Token fallback_key = { std::to_string(fallback_index++), itr->line, itr->column };

                            TokenGroup key_group(true, depth, separator_found ? sub_tokens_1 : std::vector<Token> { fallback_key });
                            TokenGroup value_group(true, depth, separator_found ? sub_tokens_2 : sub_tokens_1);

                            if(value_group.tokens.size() > 0 || itr->token.compare("]") != 0) {//only pushes if needed
                                entries.push_back(std::make_pair(key_group, value_group));
                            }

                            sub_tokens_1.clear();
                            sub_tokens_2.clear();
                            separator_found = false;
                        }
                        else if(itr->token.compare(":") == 0 && sqr_bracket_depth == 1) {//separator found
                            if(separator_found) {
                                return make_error("Duplicate ':' on dictionary creation", *itr);
                            }
                            separator_found = true;
                        }
                        else {
                            if(itr->token.compare("[") == 0) {
                                sqr_bracket_depth++;
                            }
                            (separator_found ? sub_tokens_2 : sub_tokens_1).push_back(*itr);
                        }

                        if(itr->token.compare("]") == 0 && sqr_bracket_depth == 0) {
                            if(++itr != tokens.end()) {
                                return make_error_unexpected(*itr);
                            }

                            ConstructDictionaryOperation* construct_op = new ConstructDictionaryOperation();
                            for(auto pair : entries) {
                                if(pair.first.compile(error_string) && pair.second.compile(error_string)) {
                                    construct_op->add_pair(pair.first.get_operation(), pair.second.get_operation());
                                }
                                else {
                                    // TODO: delete unused ops
                                    return false;
                                }
                            }



                            clear_error();
                            compiled = true;
                            remaining_tokens = std::vector<Token>();
                            next_depth = depth;
                            
                            operation = construct_op;
                            return true;
                        }
                    }
                }
                else {
                    return make_error("Invalid token: '%t'", first_token);
                }
            }
        }
        
        if(error_string != nullptr) {
            clear_error();
        }
        return false;
    }

    bool TokenGroup::is_compiled() const {
        return compiled;
    }

    Token TokenGroup::get_first_token() const {
        return first_token;
    }

    TokenGroupType TokenGroup::get_type() const {
        return type;
    }

    std::vector<Token> TokenGroup::get_info_tokens() const {
        if(compiled) {
            return info_tokens;
        }
        return std::vector<Token>();
    }

    std::vector<Token> TokenGroup::get_remaining_tokens() const {
        if(compiled) {
            return remaining_tokens;
        }
        return std::vector<Token>();
    }

    int TokenGroup::get_depth() const {
        if(compiled) {
            return depth;
        }
        return -1;
    }

    int TokenGroup::get_next_depth() const {
        if(compiled) {
            return next_depth;
        }
        return -1;
    }

    Operation* TokenGroup::get_operation() const {
        return operation;
    }
}