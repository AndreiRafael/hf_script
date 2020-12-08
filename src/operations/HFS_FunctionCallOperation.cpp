#include "HFS_FunctionCallOperation.hpp"

namespace hfs {
    FunctionCallOperation::FunctionCallOperation(const std::string function_name, ScriptRunner* const runner) {
        this->function_name = function_name;
        this->runner = runner;
    }

    OperationResult FunctionCallOperation::run(Scope* const scope,
                                               const std::vector<Variable> values, 
                                               Variable* const returned_value,
                                               Operation** const next_operation,
                                               Scope** next_scope) const {
        
        auto script_function = runner->get_script_function(function_name);
        if(script_function != nullptr) {//wil run script function
            Scope* new_scope = new Scope(nullptr);// TODO: o pai deste escopo deve ser, diretamente, o escoppo principal do script runner
            // TODO: Tem que achar uma forma de deletar esses caras(escopos) depois também
            auto names = runner->get_variable_names(function_name);
            for(int i = 0; i < names.size() && i < values.size(); ++i) {
                new_scope->get_variable(names[i])->set(values[i].get_raw_value());
            }

            *next_operation = script_function; //TODO: como vai funcionar a saída da função??, pra continuar a execução do código depois da chamada??
            *next_scope = new_scope;
            *returned_value = Variable::create_null();
            return OperationResult::Return;
        }

        auto bound_function = runner->get_bound_function(function_name);
        if(bound_function != nullptr) {
            *returned_value = (*bound_function)(values);
            *next_operation = nullptr;
            *next_scope = nullptr;
            return OperationResult::Return;
        }

        *returned_value = Variable::create_null();
        return OperationResult::Return;
    }
}