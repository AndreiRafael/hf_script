#ifndef HFS_TOKENGROUP_HPP
#define HFS_TOKENGROUP_HPP

#include <vector>
#include "operations/HFS_Operation.hpp"
#include "HFS_Token.hpp"

namespace hfs {
    enum TokenGroupType {
        Unknown = 0,
        DictionaryCreation,
        FunctionEntry,//simply a deepen scope operation with added function declaration info
        IfConditional, //
        ElseIfConditional, //
        ElseConditional, //
        WhileLoop,
        Set,
        Return,
        Release,
        FunctionCall,
        SubCall,
        RawValue,
        VariableRetrieval,
        DeepenScope,
        FlattenScope,
        FunctionEnd
    };

    class TokenGroup {
    private:
        bool compiled = false;

        Token first_token;
        std::vector<Token> info_tokens;//info tokens obtained after compilation
        std::vector<Token> remaining_tokens;
        int next_depth = -1;
        Operation* operation = nullptr;

        TokenGroupType type = TokenGroupType::Unknown;

        std::vector<Token> tokens;// pair<pair<line, column>, token>
        int depth;
        bool sub_group = false;
    public:
        TokenGroup(const bool sub_group, const int depth, const std::vector<Token> tokens);

        /**
         * @brief Compiles token group, attempting to determine its type and generate any necessary child groups, which are also compiled
         * 
         * @param error_string A string to be filled with error information when the function returns nullptr
         * 
         * @return A valid operation if type could be determined for self and for subsequent child groups
         * @return nullptr otherwise
         */
        bool compile(std::string* error_string);

        bool is_compiled() const;
        Token get_first_token() const;
        TokenGroupType get_type() const;
        std::vector<Token> get_info_tokens() const;
        std::vector<Token> get_remaining_tokens() const;
        int get_depth() const;
        int get_next_depth() const;
        Operation* get_operation() const;
    };
}

#endif//HFS_TOKENGROUP_HPP