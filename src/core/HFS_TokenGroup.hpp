#ifndef HFS_TOKENGROUP_HPP
#define HFS_TOKENGROUP_HPP

#include <vector>
#include "../operations/HFS_Operation.hpp"
#include "HFS_Token.hpp"

namespace hfs::core {
    enum class TokenGroupType {
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
        //obtainable values
        bool compiled = false;
        Token first_token;
        std::vector<Token> info_tokens;//info tokens obtained after compilation
        std::vector<Token> remaining_tokens;
        int next_depth = -1;
        TokenGroupType type = TokenGroupType::Unknown;

        //construction values
        std::vector<Token> tokens;// pair<pair<line, column>, token>
        int depth;
        bool sub_group = false;

        //internal values
        std::vector<TokenGroup> child_gropus;

        /**
         * @brief Compiles token group, attempting to determine its type and generate any necessary child groups, which are also compiled
         * 
         * @param error_string A string pointer to be filled with error information when the function returns false
         **/
        bool internal_compile(std::string* error_string);
    public:
        TokenGroup(const bool sub_group, const int depth, const std::vector<Token> tokens);

        bool compile(std::string* error_string);

        bool is_compiled() const;
        Token get_first_token() const;
        TokenGroupType get_type() const;
        std::vector<Token> get_info_tokens() const;
        std::vector<Token> get_remaining_tokens() const;
        int get_depth() const;
        int get_next_depth() const;
        Operation* build_operation() const;
    };
}

#endif//HFS_TOKENGROUP_HPP