#include "HFS_Essentials.hpp"
#include <algorithm>

namespace hfs {
    Variable equal(std::vector<Variable> values) {
        if(values.size() < 2) {
            return Variable::create_null();
        }

        Variable first = values[0];
        bool result = std::all_of(values.begin() + 1, values.end(), [first] (const Variable v) { return v.is_equal(first); });
        return Variable::create_boolean(result);
    }

    Variable not(std::vector<Variable> values) {
        if(values.size() != 1 || values[0].get_type() != VariableType::Boolean) {
            return Variable::create_null();
        }

        return Variable::create_boolean(!values[0].get_boolean_value());
    }

    Variable and(std::vector<Variable> values) {
        bool all_bools = std::all_of(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Boolean; });
        if(!all_bools || values.size() == 0) {
            return Variable::create_null();
        }

        bool result = std::all_of(values.begin(), values.end(), [] (const Variable v) { return v.get_boolean_value(); });
        return Variable::create_boolean(result);
    }

    Variable or(std::vector<Variable> values) {
        bool all_bools = std::all_of(values.begin(), values.end(), [] (const Variable v) { return v.get_type() == VariableType::Boolean; });
        if(!all_bools || values.size() == 0) {
            return Variable::create_null();
        }

        bool result = std::any_of(values.begin(), values.end(), [] (const Variable v) { return v.get_boolean_value(); });
        return Variable::create_boolean(result);
    }


    Variable less(std::vector<Variable> values) {
        if(values[0].is_number() && values[1].is_number()) {
            return Variable::create_boolean(values[0].get_float_value() < values[1].get_float_value());
        }
        return Variable::create_boolean(false);
    }

    Variable greater(std::vector<Variable> values) {
        if(values[0].is_number() && values[1].is_number()) {
            return Variable::create_boolean(values[0].get_float_value() > values[1].get_float_value());
        }
        return Variable::create_boolean(false);
    }
}