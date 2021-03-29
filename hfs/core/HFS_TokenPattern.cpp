#include "HFS_TokenPattern.hpp"
#include <cassert>

namespace hfs::core {
    void AuxOpVector::reset() {
        aux_operations.clear();
    }

    void AuxOpVector::set(const AuxOpType type, Operation* const op) {
        aux_operations.insert_or_assign(type, op);
    }

    Operation* AuxOpVector::get(const AuxOpType type) {
        auto itr = aux_operations.find(type);
        if(itr == aux_operations.end()) {
            return nullptr;
        }
        return itr->second;
    }


    CompilationResult::CompilationResult(std::vector<Operation*> operations, std::vector<ScriptFunctionDef> function_defs) {
        this->operations = operations;
        this->function_defs = function_defs;
    }


    PatternData::PatternData(PatternKey* const key, const int min_occurence, const int max_occurence) {
        this->key = key;
        this->min_occurence = min_occurence;
        this->max_occurence = max_occurence;
    }


    TokenPattern::TokenPattern(const PatternType type) {
        this->type = type;
    }

    void TokenPattern::add_key_data(PatternData data) {
        assert(data.min_occurence >= 0 && data.max_occurence != 0 && (data.max_occurence == -1 || data.max_occurence >= data.min_occurence));
        pattern.push_back(data);
    }

    void TokenPattern::set_compile_function(std::function<CompilationResult(MatchResult*, AuxOpVector&)> compile_function) {
        do_compile = compile_function;
    }

    MatchResult* TokenPattern::try_match(std::vector<Token>::iterator& token_itr, Token* error_token) {
        decltype(pattern.size()) group = 0;
        std::vector<MatchGroup> match_groups;
        
        Token bad_token = *token_itr;
        while(group < pattern.size()) {
            auto g = pattern[group];
            int occurences = 0;
            match_groups.push_back(MatchGroup());
            while(true) {
                auto tmp_itr = token_itr;
                auto m = g.key->match(tmp_itr);
                if(!m.matched) {
                    int line_diff = m.matched_token.line - bad_token.line;
                    if(line_diff > 0 || (line_diff == 0 && m.matched_token.column > bad_token.column)) {
                        bad_token = m.matched_token;
                    }
                    break;
                }
                token_itr = tmp_itr;
                match_groups.back().matches.push_back(m);
                if(++occurences >= g.max_occurence && g.max_occurence != -1) {
                    break;
                }
            }
            if(occurences < g.min_occurence) {
                // TODO: cleanup prev matches
                if(error_token != nullptr) {
                    *error_token = bad_token;
                }
                return nullptr;
            }
            ++group;
        }
        bad_token = Token(TokenType::Invalid);
        return new MatchResult(match_groups, this);
    }

    CompilationResult TokenPattern::compile(MatchResult* result, AuxOpVector& aux_operation) {
        if(do_compile != nullptr) {
            return do_compile(result, aux_operation);
        }
        return CompilationResult();
    }

    std::size_t TokenPattern::group_count() const {
        return pattern.size();
    }

    PatternType TokenPattern::get_type() {
        return type;
    }
}