#ifndef HFS_SCRIPT_HPP
#define HFS_SCRIPT_HPP

#include "./operations/HFS_Operation.hpp"
#include "HFS_ScriptHolder.hpp"

#include <string_view>
#include <string>
#include <vector>
#include <map>

namespace hfs {
    struct ScriptFunctionDef {
        ScriptFunctionDef(const std::string name, const std::vector<std::string> parameter_names);

        std::string name;
        std::vector<std::string> parameter_names;
    };
    /**
     * \brief Class that holds a script file in a structured manner
     * 
     */
    class Script {
    private:
        std::string error_string;//Text that informs an error with the file
        bool compiled = false;//True if script compiled successfully

        std::vector<std::pair<ScriptFunctionDef, Operation*>> functions;
        std::vector<ScriptHolder*> associated_holders;// vector kept to avoid a script going invalid inside a holder
    public:
        ~Script();

        bool load_from_file(std::string_view file_path);
        bool load_from_string(std::string_view file_path);

        Operation* find_function(const std::string_view name, const unsigned int param_count);
        std::vector<std::string> get_function_parameters(const std::string_view name, const unsigned int param_count);

        /**
         * \brief Gets a string that describes an error if the script failed to compile
         */
        std::string get_error() const;

        /**
         * \brief makes a hash of the current file
         * \retuns a valid hash on success or an empty string on failure
         */
        std::string get_hash(); // TODO:

        bool is_compiled() const;

        friend bool ScriptHolder::add_script(Script*, std::string* error_string);
    };
}

#endif