#include "HFS_Math.hpp"

#include <algorithm>
#include <string>

namespace hfs {
    Variable sum (std::vector<Variable> values) {
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
}