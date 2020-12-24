#ifndef HFS_TOKEN_HPP
#define HS_TOKEN_HPP

#include <string>

namespace hfs::core {
    struct Token {
        std::string token;
        int line;
        int column;
    };
}

#endif//HFS_TOKEN_HPP