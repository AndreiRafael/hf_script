#ifndef HFS_UTILITY_HPP
#define HFS_UTILITY_HPP

#include <string>

namespace hfs::core {
    enum TokenType;
}

namespace hfs::utility {
    bool is_operator_symbol(const char& c);
    bool is_special_symbol(const char& c);
    bool is_special_token(const std::string& token);
    bool is_raw_value(const std::string& token);
    bool validate_name(const std::string& name);
    core::TokenType determine_token_type(const std::string& token);
}

#endif