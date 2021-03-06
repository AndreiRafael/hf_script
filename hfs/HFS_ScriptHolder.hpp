#ifndef HFS_SCRIPTHOLDER_HPP
#define HFS_SCRIPTHOLDER_HPP

#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>

#include "./core/HFS_OperationRunner.hpp"
#include "./core/HFS_Operator.hpp"

namespace hfs {
    class Script;

    struct BoundFunctionDef {
        std::string name;
        int param_count;
    };

    struct ReturnData {
        bool success;
        unsigned int id;
        Variable value;
    };

    typedef std::vector<Variable> ParameterList;//TODO: use refs for better performance??
    typedef std::function<Variable(ParameterList)> BindableFunction;

    class ScriptHolder {
    private:
        Scope* scope;

        std::vector<ScriptHolder*> subordinate_script_holders = std::vector<ScriptHolder*>();//holders to inform in case this gets destroyed
        std::vector<ScriptHolder*> superior_script_holders = std::vector<ScriptHolder*>();//holders to look for functions in case this fails to find
        
        std::unordered_map<unsigned int, core::OperationRunner*> operation_runners;
        std::vector<ReturnData> return_data = std::vector<ReturnData>();

        //two maps bc bound or script funcs may be removed midway through execution
        std::vector<std::pair<BoundFunctionDef, BindableFunction>> bound_functions;
        std::vector<std::pair<BoundFunctionDef, Operation*>> bound_operations;//a cheap way to delay direct calls from start_function by a frame

        std::vector<core::Operator> operators;//all defined operators

        std::vector<Script*> scripts;

        int operation_limit = -1;

        unsigned int id_gen = 0;

        bool create_function_runner (const std::string_view function_name, const std::vector<Variable> parameters, Scope* top_scope, core::OperationRunner** new_runner);
        bool has_superior_holder(ScriptHolder* holder) const;
        bool step_runner(core::OperationRunner* runner, unsigned int runner_id, ReturnData* return_data);//returns true if operation retuned

        static bool validate_operator(std::string const& symbol);

    public:
        ScriptHolder();
        ScriptHolder(ScriptHolder const&) = delete;
        ~ScriptHolder();

        /**
         * @brief 
         * 
         * @param script 
         * @param error_string 
         * @return true 
         * @return false 
         */
        bool add_script(Script* script, std::string* const error_string = nullptr);

        /**
         * @brief Removes a valid script from the holder
         * 
         * @param script The script to remove from the holder
         * @return true if the script is valid and was associated with the holder
         * @return false otherwise
         * @remarks This is called internaly by the destructor of @ref hfs::Script
         */
        bool remove_script(Script* script);

        bool add_superior_holder(ScriptHolder* holder);
        bool remove_superior_holder(ScriptHolder* holder);

        /**
         * @brief Set the maximum number of operations that can be ran in a single step, to avoid infinite loops
         * @remarks If a step fails, execution will be stopped, and the returned data will have its success boolean set to false
         * @param amount The number of operations allowed to be ran in a step. Values lower than or equal to 0 mean there's no limit
         */
        void set_operation_limit(const int amount);

        /**
         * @brief Binds a c++ function to a string that can be called inside scripts
         * 
         * @param function_name 
         * @param function 
         * @param param_count 
         * @return true if bounded the function correctly
         * @return false if there was already another function with the same name and parameter count bound
         */
        bool bind_function(std::string function_name, BindableFunction function, int param_count = -1);// TODO: min/max param range 

        BindableFunction* get_bound_function(const std::string_view function_name, const int param_count);
        Operation* get_script_function(const std::string_view function_name, const unsigned int param_count);

        /**
         * \brief From a function name, gets the parameter names as defined in the script
         * \returns An empty vector on error, or a vector with the variable names
         */
        std::vector<std::string> get_parameter_names(const std::string_view function_name, const unsigned int param_count);

        /**
         * @brief Get an operator given its symbol
         * 
         * @param symbol A string composed of one or more valid operator characters
         * @param type The type of operator to get, single or double
         * @param op A pointer to an operator to be filled with the operator if function returns true
         * @return true if operator exists
         * @return false if operator does not exist or symbol uses invalid characters
         */
        bool get_operator(const std::string symbol, const core::OperatorType type, core::Operator* op) const;

        bool define_operator(const std::string symbol, const std::string function, const core::OperatorType type);
        bool undefine_operator(const std::string symbol, const core::OperatorType type);

        /**
         * @brief Runs a function until it returns or is released. In the latter case, the function is later resumed in \ref ScriptHolder::step
         * @param function_name The name of the function
         * @param parameters The parameters to be passed to the function
         * @returns An id, used to get the return value with \ref ScriptHolder::get_returned_value
         */
        unsigned int start_function(const std::string_view function_name, const std::vector<Variable> parameters);

        /**
         * @brief Runs a function instantly, skipping release calls
         * @param function_name The name of the function
         * @param parameters The parameters to be passed to the function
         * @param returned_value A pointer to a variable which is filled with the returned value if the function successfully run
         * @remarks If not used carefully, this may allow infinite loops
         */
        bool run_function(std::string_view function_name, const std::vector<Variable> parameters, Variable* returned_value);
        
        /**
         * \brief Steps queued functions once, any function that finishes execution has its return value returned
         * \returns An array of ids and variables returned from functions that ended this call
         */
        std::vector<ReturnData> step();

        /**
         * @brief Get the returned data from function calls. Once a function returns value, it is stored until @ref ScriptHolder::step is called again
         * 
         * @param returned_data A pointer to be filled with relevant information in case the function returns true
         * @return true is the data for the desired id exists
         * @return false otherwise
         */
        bool get_returned_data(unsigned int function_id, ReturnData* returned_data);

        /**
         * @brief Get the scope of this ScriptHolder. The scope created by this holder on construction
         * 
         * @remarks Do not delete this Scope, it is the responsability of the Scope to do so
         */
        Scope* get_scope() const;    // TODO: this now belongs to scrpt Runner, holder is just a collection
    };
}

#endif