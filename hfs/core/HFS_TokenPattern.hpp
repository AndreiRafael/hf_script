#ifndef HFS_TOKENPATTERN_HPP
#define HFS_TOKENPATTERN_HPP

#include "HFS_PatternKeys.hpp"
#include "../HFS_Script.hpp"
#include <functional>
#include <unordered_map>
#include <stack>

namespace hfs::core {
    enum class AuxOpType {
        Next,
        LoopEntry,
        LoopExit
    };

    struct AuxOp {
        Operation* op;
        int depth;

        AuxOp(Operation* const op, const int depth);
    };

    struct AuxOpVector {
    private:
         std::unordered_map<AuxOpType, std::stack<AuxOp>> aux_operations = std::unordered_map<AuxOpType, std::stack<AuxOp>>();

    public:
        void reset();
        void push(const AuxOpType type, const AuxOp op);
        void pop(const AuxOpType type);
        AuxOp get(const AuxOpType type, const bool pop = true);
    };


    struct CompilationData {
        int depth;
        AuxOpVector aux_operations;

        CompilationData(const int depth = 0, const AuxOpVector auxoperaions = AuxOpVector());
    };

    struct CompilationResult {
        CompilationResult(std::vector<Operation*> operations = {}, std::vector<ScriptFunctionDef> function_defs = {});

        std::vector<Operation*> operations;
        std::vector<ScriptFunctionDef> function_defs;
    };

    struct PatternData {
        PatternData(PatternKey* const key, const int min_occurence = 1, const int max_occurence = 1);

        PatternKey* key;
        int min_occurence;
        int max_occurence;
    };

    class TokenPattern {
    private:
        std::vector<PatternData> pattern;

        std::function<CompilationResult(MatchResult*, CompilationData&)> do_compile = nullptr;
        PatternType type;
    public:
        TokenPattern(const PatternType type);

        void add_key_data(PatternData data);
        void set_compile_function(std::function<CompilationResult(MatchResult* result, CompilationData& aux_operation)> compile_function);

        MatchResult* try_match(std::vector<TokenMatcher>::iterator& token_itr, Token* error_token);
        
        /**
         * @brief Compiles Pattern and sub patterns into an array of operations
         */
        CompilationResult compile(MatchResult* result, CompilationData& aux_operations);

        std::size_t group_count() const;
        PatternType get_type();
    };
}

#endif