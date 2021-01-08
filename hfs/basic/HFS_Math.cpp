#include "HFS_Math.hpp"

#include <cmath>
#include <algorithm>
#include <string>

#include "../HFS_ScriptHolder.hpp"

namespace hfs {
    namespace basic::math {
        Variable sum (ParameterList values) {//-1
            auto num_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.is_number(); });
            auto float_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Float; });

            if(values.size() == 0 || num_count != values.size()) {
                return Variable::create_null();
            }

            if(float_count > 0) {
                float res = 0.f;
                std::for_each(values.begin(), values.end(), [&res] (Variable v) { res += v.get_float_value(); });
                return Variable::create_float(res);
            }
            
            int res = 0;
            std::for_each(values.begin(), values.end(), [&res] (Variable v) { res += v.get_integer_value(); });
            return Variable::create_integer(res);
        }

        Variable subtract (ParameterList values) {//-1
            auto num_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.is_number(); });
            auto float_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Float; });

            if(values.size() == 0 || num_count != values.size()) {
                return Variable::create_null();
            }

            if(float_count > 0) {
                float res = values[0].get_float_value();
                std::for_each(values.begin() + 1, values.end(), [&res] (Variable v) { res -= v.get_float_value(); });
                return Variable::create_float(res);
            }
            
            int res = values[0].get_integer_value();
            std::for_each(values.begin() + 1, values.end(), [&res] (Variable v) { res -= v.get_integer_value(); });
            return Variable::create_integer(res);
        }

        Variable multiply (ParameterList values) {//-1
            auto num_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.is_number(); });
            auto float_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Float; });

            if(values.size() == 0 || num_count != values.size()) {
                return Variable::create_null();
            }

            if(float_count > 0) {
                float res = values[0].get_float_value();
                std::for_each(values.begin() + 1, values.end(), [&res] (Variable v) { res *= v.get_float_value(); });
                return Variable::create_float(res);
            }
            
            int res = values[0].get_integer_value();
            std::for_each(values.begin() + 1, values.end(), [&res] (Variable v) { res *= v.get_integer_value(); });
            return Variable::create_integer(res);
        }

        Variable divide (ParameterList values) {//-1
            auto num_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.is_number(); });
            auto float_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Float; });

            if(values.size() == 0 || num_count != values.size()) {
                return Variable::create_null();
            }

            if(float_count > 0) {
                float res = values[0].get_float_value();
                std::for_each(values.begin() + 1, values.end(), [&res] (Variable v) { res /= v.get_float_value(); });
                return Variable::create_float(res);
            }
            
            int res = values[0].get_integer_value();
            std::for_each(values.begin() + 1, values.end(), [&res] (Variable v) { res /= v.get_integer_value(); });
            return Variable::create_integer(res);
        }

        Variable pow (ParameterList values) {//2
            const auto num_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.is_number(); });
            const auto float_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Float; });

            if(num_count != values.size()) {
                return Variable::create_null();
            }

            if(float_count > 0) {
                const float res = std::powf(values[0].get_float_value(), values[1].get_float_value());
                return Variable::create_float(res);
            }
            
            const int res = std::pow(values[0].get_integer_value(), values[1].get_integer_value());
            return Variable::create_integer(res);
        }

        Variable sqrt (ParameterList values) {//1
            const auto num_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.is_number(); });

            if(num_count != values.size()) {
                return Variable::create_null();
            }

            const float res = std::sqrtf(values[0].get_float_value());
            return Variable::create_float(res);
        }

        void apply(ScriptHolder* holder) {
            holder->bind_function("sum", BindableFunction(sum), -1);
            holder->bind_function("subtract", BindableFunction(subtract), -1);
            holder->bind_function("multiply", BindableFunction(multiply), -1);
            holder->bind_function("divide", BindableFunction(divide), -1);
            
            holder->bind_function("pow", BindableFunction(pow), 2);
            holder->bind_function("sqrt", BindableFunction(sqrt), 1);
        }
    }
}