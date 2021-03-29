#include "HFS_PatternKeys.hpp"
#include "HFS_TokenPattern.hpp"
#include <algorithm>
#include <cassert>

namespace hfs::core {
    Match::Match(const bool matched, const Token matched_token) {
        this->matched = matched;
        this->matched_token = matched_token;
        this->sub_match = nullptr;
    }

    Match::Match(const bool matched, MatchResult* const sub_match) {
        this->matched = matched;
        this->matched_token = Token(TokenType::Invalid);
        this->sub_match = sub_match;
    }

    Match Match::failed_match(const Token error_token) {
        return Match(false, error_token);
    }


    MatchGroup::MatchGroup(const std::vector<Match> matches) {
        this->matches = matches;
    }

    MatchGroup::MatchGroup() : MatchGroup::MatchGroup(std::vector<Match>()) { }


    MatchResult::MatchResult(const std::vector<MatchGroup> groups, TokenPattern* const pattern) {
        this->groups = groups;
        this->pattern = pattern;
    }


    TextPatternKey::TextPatternKey(const std::string text) {
        this->text = text;
    }

    Match TextPatternKey::match(std::vector<Token>::iterator& token_itr) const {
        if(token_itr->token.compare(text) == 0) {
            return Match(true, *token_itr++);
        }
        return Match::failed_match(*token_itr);
    }


    TokenTypePatternKey::TokenTypePatternKey(const TokenType type) {
        this->type = type;
    }

    Match TokenTypePatternKey::match(std::vector<Token>::iterator& token_itr) const {
        if(token_itr->type == type) {
            return Match(true, *token_itr++);
        }
        return Match::failed_match(*token_itr);
    }


    OtherPatternKey::OtherPatternKey(TokenPattern* pattern) {
        this->pattern = pattern;
    }

    Match OtherPatternKey::match(std::vector<Token>::iterator& token_itr) const {
        Token bad_token = Token(TokenType::Invalid);
        auto res = pattern->try_match(token_itr, &bad_token);
        if(res != nullptr) {
            return Match(true, res);
        }
        return Match::failed_match(bad_token);
    }


    PatternListPatternKey::PatternListPatternKey(PatternType type_mask, std::vector<TokenPattern*> patterns) {
        this->patterns = patterns;
        this->type_mask = type_mask;
    }

    Match PatternListPatternKey::match(std::vector<Token>::iterator& token_itr) const {
        auto cpy = patterns;
        std::sort(cpy.begin(), cpy.end(), [] (TokenPattern* a, TokenPattern* b) {
            return a->group_count() > b->group_count();
        });

        Token bad_token = Token(TokenType::Invalid);
        for(auto itr = cpy.begin(); itr != cpy.end(); ++itr) {
            if(((*itr)->get_type() & type_mask) == 0) {
                continue;//skips unwanted types
            }

            auto tmp_itr = token_itr;
            Token tmp_bad_token;
            auto res = (*itr)->try_match(tmp_itr, &tmp_bad_token);
            if(res != nullptr) {
                token_itr = tmp_itr;
                return Match(true, res);
            }
            int line_diff = tmp_bad_token.line - bad_token.line;
            if(line_diff > 0 || (line_diff == 0 && tmp_bad_token.column > bad_token.column)) {
                bad_token = tmp_bad_token;
            }
        }
        return Match::failed_match(bad_token);
    }
}