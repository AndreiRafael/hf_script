#ifndef HFS_SCRIPT_HPP
#define HFS_SCRIPT_HPP

#include "operations/HFS_Operation.hpp"
#include <string_view>
#include <string>
#include <vector>
#include <unordered_map>

namespace hfs {
    /**
     * \brief Class that holds a script file in a structured manner
     * 
     */
    class Script {
    private:
        std::string error_string;//Text that informs an error with the file
        bool compiled = false;//True if script compiled successfully

        std::unordered_map<std::string, std::pair<Operation*, std::vector<std::string>>> functions;

    public:
        /**
         * \brief Validates a full text of a script
         */
        static bool validate_text(const std::string text);
        /**
         * \brief Validates a single script call
         */
        static bool validate_call(const std::string text);


        bool load_from_file(std::string_view file_path);
        bool load_from_string(std::string_view file_path);

        /**
         * \brief Gets a string that describes an error if the script failed to compile
         */
        std::string get_error() const;

        /**
         * \brief makes a hash of the current file
         * \retuns a valid hash on success or an empty string on failure
         */
        std::string get_hash();
    };
}

#endif