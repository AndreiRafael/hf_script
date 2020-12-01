#ifndef HFS_SCRIPTRUNNER_HPP
#define HFS_SCRIPTRUNNER_HPP

#include "HFS_OperationRunner.hpp"
#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>

// TODO: restructure this class, so that it no longer loads files, but keeps an array of Scripts
namespace hfs {
    class ScriptRunner {
    private:
        std::unordered_map<unsigned int, OperationRunner*> operation_runners;

        //two maps bc bound or script funcs may be removed midway through execution
        std::unordered_map<std::string, std::function<Variable(std::vector<Variable>)>> bound_functions;
        std::unordered_map<std::string, Operation*> bound_operations;//a cheap way to delay direct calls from start_function by a frame

        std::unordered_map<std::string, Operation*> script_functions;
        std::unordered_map<std::string, std::vector<std::string>> function_parameters;

        unsigned int id_gen = 0;

    public:
        bool load_script(std::string_view file_path, std::string* const error_string = nullptr);

        /**
         * \brief Binds a c++ function to a string that can be called inside scripts 
         */
        bool bind_function(std::string function_name, std::function<Variable(std::vector<Variable>)> function);

        std::function<Variable(std::vector<Variable>)>* get_bound_function(const std::string_view function_name);
        Operation* get_script_function(const std::string_view function_name);

        /**
         * \brief From a function name, gets the parameter names as defined in the script
         * \returns An empty vector on error, or a vector with the variable names
         */
        std::vector<std::string> get_variable_names(const std::string_view function_name);

        /**
         * \brief Adds a function with to the run queue, actual running happens in \ref ScriptRunner::step
         * \remarks Does not work with bound functions
         * \param function_name The name of the function
         * \param parameters The static parameters to be passed to the function
         * \returns An id, used to get the return value from \ref ScriptRunner::step
         */
        unsigned int start_function(const std::string_view function_name, const std::vector<Variable> parameters);

        /**
         * \brief Runs a function instantly, jumping wait calls
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