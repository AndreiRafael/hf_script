#include "HFS_Essentials.hpp"
#include "../HFS_ScriptRunner.hpp"
#include <algorithm>

namespace hfs {
    namespace basic::essentials {
        Variable equal(std::vector<Variable> values) {//-1
            bool result = std::all_of(values.begin() + 1, values.end(), [values] (const Variable v) { return v.is_equal(*values.begin()); });
            return Variable::create_boolean(result);
        }

        Variable not(std::vector<Variable> values) {//1
            if(values.size() != 1 || values[0].get_type() != VariableType::Boolean) {
                return Variable::create_null();
            }

            return Variable::create_boolean(!values[0].get_boolean_value());
        }

        Variable and(std::vector<Variable> values) {//-1
            bool all_bools = std::all_of(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Boolean; });
            if(!all_bools || values.size() == 0) {
                return Variable::create_null();
            }

            bool result = std::all_of(values.begin(), values.end(), [] (const Variable v) { return v.get_boolean_value(); });
            return Variable::create_boolean(result);
        }

        Variable or(std::vector<Variable> values) {//-1
            bool all_bools = std::all_of(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Boolean; });
            if(!all_bools || values.size() == 0) {
                return Variable::create_null();
            }

            bool result = std::any_of(values.begin(), values.end(), [] (const Variable v) { return v.get_boolean_value(); });
            return Variable::create_boolean(result);
        }


        Variable less(std::vector<Variable> values) {//2
            if(values[0].is_number() && values[1].is_number()) {
                return Variable::create_boolean(values[0].get_float_value() < values[1].get_float_value());
            }
            return Variable::create_boolean(false);
        }

        Variable lequal(std::vector<Variable> values) {//2
            if(values[0].is_number() && values[1].is_number()) {
                return Variable::create_boolean(values[0].get_float_value() <= values[1].get_float_value());
            }
            return Variable::create_boolean(false);
        }

        Variable greater(std::vector<Variable> values) {//2
            if(values[0].is_number() && values[1].is_number()) {
                return Variable::create_boolean(values[0].get_float_value() > values[1].get_float_value());
            }
            return Variable::create_boolean(false);
        }

        Variable gequal(std::vector<Variable> values) {//2
            if(values[0].is_number() && values[1].is_number()) {
                return Variable::create_boolean(values[0].get_float_value() >= values[1].get_float_value());
            }
            return Variable::create_boolean(false);
        }


        Variable typeof(std::vector<Variable> values) {//1
            if(values.size() != 1) {
                return Variable::create_null();
            }

            std::string str = "";
            switch (values[0].get_type())
            {
            case VariableType::Boolean:
                str = "bool";
                break;
            case VariableType::Dictionary:
                str = "dictionary";
                break;
            case VariableType::Float:
                str = "float";
                break;
            case VariableType::Integer:
                str = "int";
                break;
            case VariableType::Null:
                str = "null";
                break;
            case VariableType::String:
                str = "string";
                break;
            }

            return Variable::create_string(str);
        }


        Variable strcmp(std::vector<Variable> values) {//2
            if(values.size() != 2) {
                return Variable::create_boolean(false);
            }

            return Variable::create_boolean(values[0].get_string_value().compare(values[1].get_string_value()) == 0);
        }

        Variable strlen(std::vector<Variable> values) {//1
            return Variable::create_integer(values[0].get_string_value().size());
        }

        Variable strcat(std::vector<Variable> values) {//-1
            std::string str = "";
            for(auto v : values) {
                str.append(v.get_string_value());
            }

            return Variable::create_string(str);
        }

        Variable substr(std::vector<Variable> values) {//3
            return Variable::create_string(values[0].get_string_value().substr(values[1].get_integer_value(), values[2].get_integer_value()));
        }


        Variable random(std::vector<Variable> values) {//2
            int a = values[0].get_integer_value();
            int b = values[1].get_integer_value();

            int min = std::min(a, b);
            int max = std::max(a, b);

            int diff = max - min;
        
            return Variable::create_integer(diff == 0 ? min : (std::rand() % diff) + min);
        }


        void apply(ScriptRunner* runner) {
            runner->bind_function("equal", std::function<Variable(std::vector<Variable>)>(equal));
            runner->bind_function("not", std::function<Variable(std::vector<Variable>)>(not), 1);
            runner->bind_function("and", std::function<Variable(std::vector<Variable>)>(and));
            runner->bind_function("or", std::function<Variable(std::vector<Variable>)>(or));

            runner->bind_function("less", std::function<Variable(std::vector<Variable>)>(less), 2);
            runner->bind_function("lequal", std::function<Variable(std::vector<Variable>)>(lequal), 2);
            runner->bind_function("greater", std::function<Variable(std::vector<Variable>)>(greater), 2);
            runner->bind_function("gequal", std::function<Variable(std::vector<Variable>)>(gequal), 2);

            runner->bind_function("typeof", std::function<Variable(std::vector<Variable>)>(typeof), 1);

            runner->bind_function("strcmp", std::function<Variable(std::vector<Variable>)>(strcmp), 2);
            runner->bind_function("strlen", std::function<Variable(std::vector<Variable>)>(strlen), 1);
            runner->bind_function("strcat", std::function<Variable(std::vector<Variable>)>(strcat));
            runner->bind_function("substr", std::function<Variable(std::vector<Variable>)>(substr), 3);
            
            runner->bind_function("random", std::function<Variable(std::vector<Variable>)>(random), 2);
        }
    }
}