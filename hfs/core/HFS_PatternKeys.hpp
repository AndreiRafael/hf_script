#ifndef HFS_PATTERNKEYS_HPP
#define HFS_PATTERNKEYS_HPP

#include <vector>
#include "HFS_Token.hpp"
#include "../operations/HFS_Operation.hpp"

namespace hfs::core {
    class TokenPattern;

    struct MatchResult;
    struct Match {
        Match(const bool matched, const Token matched_token);
        Match(const bool matched, MatchResult* const sub_match);

        static Match failed_match(const Token error_token);

        bool matched;
        Token matched_token;
        MatchResult* sub_match;// TODO: who owns this pointer, does it have to be a pointer, does it have to be a MatchResult?
    };

    struct MatchGroup {
        MatchGroup(const std::vector<Match> matches);
        MatchGroup();

        std::vector<Match> matches;
    };

    /**
     * @brief The result of a match, containing matched groups and the pattern that made this match     * 
     */
    struct MatchResult {
        MatchResult(const std::vector<MatchGroup> groups, TokenPattern* const pattern);

        std::vector<MatchGroup> groups;
        TokenPattern* pattern;
    };

    enum PatternType {
        None       = 0b000,
        Definition = 0b001,
        Value      = 0b010,
        Void       = 0b100,
        Any        = 0b111
    };

    class PatternKey {
    public:
        virtual Match match(std::vector<Token>::iterator& token_itr) const = 0;
    };

    /**
     * @brief Key that matches if token is of given type
     */
    class TokenTypePatternKey: public PatternKey {
    private:
        TokenType type;
    public:
        TokenTypePatternKey(const TokenType type);

        Match match(std::vector<Token>::iterator& token_itr) const final;
    };

    /**
     * @brief Key that matches an exact text
     */
    class TextPatternKey: public PatternKey {
    private:
        std::string text;
    public:
        TextPatternKey(const std::string text);

        Match match(std::vector<Token>::iterator& token_itr) const final;
    };    

    /**
     * @brief Key that matches the whole of a single Pattern
     */
    class OtherPatternKey: public PatternKey {
    private:
        TokenPattern* pattern = nullptr;
    public:
        OtherPatternKey(TokenPattern* pattern);

        Match match(std::vector<Token>::iterator& token_itr) const final;
    };

    /**
     * @brief Key that matches the whole of another Pattern from a given a pointer list
     */
    class PatternListPatternKey: public PatternKey {
    private:
        std::vector<TokenPattern*> patterns;
        PatternType type_mask = PatternType::Any;
    public:
        PatternListPatternKey(PatternType type_mask, std::vector<TokenPattern*> patterns);

        Match match(std::vector<Token>::iterator& token_itr) const final;
    };
}

#endif