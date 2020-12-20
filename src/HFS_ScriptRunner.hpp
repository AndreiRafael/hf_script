#ifndef HFS_SCRIPTRUNNER_HPP
#define HFS_SCRIPTRUNNER_HPP

#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>

#include "HFS_Script.hpp"
#include "HFS_OperationRunner.hpp"

// TODO: restructure this class, so that it no longer loads files, but keeps an array of Scripts
namespace hfs {
    struct BoundFunctionDef {
        std::string name;
        int param_count;
    };

    class ScriptRunner {
    private:
        std::unordered_map<unsigned int, OperationRunner*> operation_runners;

        //two maps bc bound or script funcs may be removed midway through execution
        std::vector<std::pair<BoundFunctionDef, std::function<Variable(std::vector<Variable>)>>> bound_functions;
        std::vector<std::pair<BoundFunctionDef, Operation*>> bound_operations;//a cheap way to delay direct calls from start_function by a frame

        std::vector<Script*> scripts;

        unsigned int id_gen = 0;

    public:
        bool add_script(Script* script, std::string* const error_string = nullptr);

        /**
         * @brief Binds a c++ function to a string that can be called inside scripts
         * 
         * @param function_name 
         * @param function 
         * @param param_count 
         * @return true if bounded the function correctly
         * @return false if there was already another function with the same name and parameter count bound
         */
        bool bind_function(std::string function_name, std::function<Variable(std::vector<Variable>)> function, int param_count = -1);

        std::function<Variable(std::vector<Variable>)>* get_bound_function(const std::string_view function_name, const int param_count);
        Operation* get_script_function(const std::string_view function_name, const unsigned int param_count);

        /**
         * \brief From a function name, gets the parameter names as defined in the script
         * \returns An empty vector on error, or a vector with the variable names
         */
        std::vector<std::string> get_parameter_names(const std::string_view function_name, const unsigned int param_count);

        /**
         * @brief Adds a function with to the run queue, actual running happens in \ref ScriptRunner::step
         * @param function_name The name of the function
         * @param parameters The static parameters to be passed to the function
         * @returns An id, used to get the return value from \ref ScriptRunner::step
         */
        unsigned int start_function(const std::string_view function_name, const std::vector<Variable> parameters);

        /**
         * \brief Runs a function instantly, jumping release calls
         * \param function_name The name of the function
         */
        Variable run_function(std::string_view function_name, const std::vector<Variable> parameters);
        
        /**
         * \brief Steps queued functions once, any function that finishes execution has its return value returned
         * \returns An array of ids and variables returned from functions that ended this call
         */
        std::vector<std::pair<unsigned int, Variable>> step();        
    };
}

#endif