#ifndef HFS_PATTERNKEYS_HPP
#define HFS_PATTERNKEYS_HPP

#include <vector>
#include "HFS_Token.hpp"
#include "../operations/HFS_Operation.hpp"
#include "HFS_Operator.hpp"

class ScriptCompiler;
namespace hfs::core {
    class TokenPattern;
    class PatternKey;

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


    /**
     * @brief Contains a token and a list of patterns that failed to match that token
     * 
     */
    struct TokenMatcher {
        Token token;
        std::vector<PatternKey*> failed_keys;

        TokenMatcher(Token token);

        bool has_failed(PatternKey* key);
    };

    enum PatternType {
        None       = 0b000,
        Definition = 0b001,
        Value      = 0b010,
        Void       = 0b100,
        Any        = 0b111
    };

    class PatternKey {
    protected:
        virtual Match internal_match(std::vector<TokenMatcher>::iterator& token_itr) const = 0;
    public:
        Match match(std::vector<TokenMatcher>::iterator& token_itr);
    };

    /**
     * @brief Key that matches if token is of given type
     */
    class TokenTypePatternKey: public PatternKey {
    private:
        TokenType type;
    protected:
        Match internal_match(std::vector<TokenMatcher>::iterator& token_itr) const final;
    public:
        TokenTypePatternKey(const TokenType type);
    };

    /**
     * @brief Key that matches an exact text
     */
    class TextPatternKey: public PatternKey {
    private:
        std::string text;
    protected:
        Match internal_match(std::vector<TokenMatcher>::iterator& token_itr) const final;
    public:
        TextPatternKey(const std::string text);
    };

    /**
     * @brief Key that matches the whole of a single Pattern
     */
    class OtherPatternKey: public PatternKey {
    private:
        TokenPattern* pattern = nullptr;
    protected:
        Match internal_match(std::vector<TokenMatcher>::iterator& token_itr) const final;
    public:
        OtherPatternKey(TokenPattern* pattern);
    };

    /**
     * @brief Key that matches the whole of another Pattern from a given a pointer list
     */
    class PatternListPatternKey: public PatternKey {
    private:
        std::vector<TokenPattern*> patterns;
        PatternType type_mask = PatternType::Any;
    protected:
        Match internal_match(std::vector<TokenMatcher>::iterator& token_itr) const final;
    public:
        PatternListPatternKey(PatternType type_mask, std::vector<TokenPattern*> patterns);
    };
}

#endif