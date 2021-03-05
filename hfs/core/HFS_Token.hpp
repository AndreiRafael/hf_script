#ifndef HFS_TOKEN_HPP
#define HS_TOKEN_HPP

#include <string>

namespace hfs::core {
    enum class TokenType {
        Invalid = -1,//error
        Symbol,      //( [ { ;
        Special,     //func return release break
        Operator,    //== + -, etc
        SetOperator, //= exclusevely
        Name,        //func or var names
        RawValue     //true, false, null, numbers
    };

    struct Token {
        TokenType type;
        std::string token;
        unsigned int line;
        unsigned int column;
    };
}

#endif//HFS_TOKEN_HPP