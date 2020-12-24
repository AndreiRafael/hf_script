#include "HFS_Math.hpp"

#include <algorithm>
#include <string>

#include "../HFS_ScriptRunner.hpp"

namespace hfs {
    namespace basic::math {
        Variable sum (std::vector<Variable> values) {//-1
            auto num_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.is_number(); });
            auto float_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Float; });

            if(num_count != values.size()) {
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

        Variable subtract (std::vector<Variable> values) {//-1
            auto num_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.is_number(); });
            auto float_count = std::count_if(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Float; });

            if(num_count != values.size()) {
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

        void apply(ScriptRunner* runner) {
            runner->bind_function("sum", std::function<Variable(std::vector<Variable>)>(sum), -1);
            runner->bind_function("subtract", std::function<Variable(std::vector<Variable>)>(subtract), -1);
        }
    }
}