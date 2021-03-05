#ifndef HFS_SCRIPTCOMPILER_HPP
#define HFS_SCRIPTCOMPILER_HPP

#include <string>
#include <vector>
#include "core/HFS_Token.hpp"

namespace hfs {
    enum class OperatorType {
        None   = 0b00,
        Single = 0b01,
        Double = 0b10,
        Any    = 0b11
    };

    struct Operator {
        std::string symbol;
        std::string function;
        OperatorType type;
    };

    enum class LogType {
        Info = 0,
        Warning,
        Error
    };

    struct LogData {
        LogType type;
        std::string text;
    };

    class ScriptCompiler {
    private:
        std::vector<LogData> logs;//error/warning logs
        std::vector<Operator> operators;//all defined operators

        static bool is_token_separator(char const& c);

        static bool validate_operator(std::string const& symbol);

        void push_log(const LogType type, const std::string text, core::Token const& token);

    public:
        /**
         * @brief Get an operator given its symbol
         * 
         * @param symbol A string composed of one or more valid operator characters
         * @param type The type of operator to get, single or double
         * @param op A pointer to an operator to be filled with the operator if function returns true
         * @return true if operator exists
         * @return false if operator does not exist or symbol uses invalid characters
         */
        bool get_operator(const std::string symbol, const OperatorType type, Operator* op) const;

        bool define_operator(const std::string symbol, const std::string function, const OperatorType type);
        bool undefine_operator(const std::string symbol, const OperatorType type);

        std::vector<core::Token> tokenize_from_file(std::string path);
        std::vector<core::Token> tokenize_from_text(std::string text);
    };
}

#endif