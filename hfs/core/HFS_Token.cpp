#include "HFS_Token.hpp"

namespace hfs::core {
    Token::Token(const TokenType type, const std::string token, const unsigned int line, const unsigned int column) {
        this->type = type;
        this->token = token;
        this->line = line;
        this->column = column;
    }

    Token::Token() : Token(TokenType::Invalid) { }
}