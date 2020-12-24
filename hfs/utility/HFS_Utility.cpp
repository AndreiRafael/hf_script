#include "HFS_Utility.hpp"

#include <algorithm>

namespace hfs::utility {
    //validates wheter a string may be used as a variable or function name
    bool validate_name(const std::string& name) {
        if(name.compare("release") == 0 ||
           name.compare("true") == 0 ||
           name.compare("false") == 0 ||
           name.compare("func") == 0 ||
           name.compare("return") == 0 ||
           name.compare("null") == 0 ||
           name.compare("else") == 0 ||
           name.compare("if") == 0 ||
           name.compare("while") == 0)
        {
            return false;
        }

        if(name.size() > 0) {
            auto itr = name.begin();
            if(*itr == '_' || (*itr >= 65 && *itr <= 90) || (*itr >= 97 && *itr <= 122)) {//first char cannot be a number
                return std::all_of(++itr, name.end(), [] (char c) { 
                                                        return c == '_' ||
                                                                (c >= 65 && c <= 90) ||
                                                                (c >= 97 && c <= 122) ||
                                                                (c >= 48 && c <= 57);
                                                    }
                );
            }
        }
        return false;
    }
}