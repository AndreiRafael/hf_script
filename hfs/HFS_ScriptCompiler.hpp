#ifndef HFS_SCRIPTCOMPILER_HPP
#define HFS_SCRIPTCOMPILER_HPP

#include <string>
#include <vector>
#include "core/HFS_Token.hpp"
#include "core/HFS_TokenPattern.hpp"

namespace hfs {
    enum class LogType {
        Info = 0,
        Warning,
        Error
    };

    struct LogData {
        LogType type;
        std::string text;
    };

    class Script;
    class ScriptCompiler {
    private:
        std::vector<LogData> logs;//error/warning logs

        core::TokenPattern* main_pattern = nullptr;

        static bool is_token_separator(char const& c);
        
        void push_log(const LogType type, const std::string text, core::Token const& token);

    public:
        ScriptCompiler(bool default_patterns = true);

        std::vector<core::TokenMatcher> tokenize_from_file(std::string path);
        std::vector<core::TokenMatcher> tokenize_from_text(std::string text);
    };
}

#endif