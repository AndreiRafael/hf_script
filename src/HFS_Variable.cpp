#include "HFS_Variable.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace hfs {
    Variable::Variable(const std::string value) {
        set(value);
    }

    VariableType Variable::determine_type(const std::string value) {
        if(value.compare("null") == 0) {
            return VariableType::Null;
        }

        auto num_count = std::count_if(value.begin(), value.end(), [] (char c) { return c >= 48 && c <=57; });
        auto dot_count = std::count_if(value.begin(), value.end(), [] (char c) { return c == 46; });

        if((num_count + dot_count) == value.size() && dot_count <= 1) {//definetly a number
            if(dot_count == 1) {
                return VariableType::Float;
            }
            return VariableType::Integer;
        }
        if(value.compare("false") == 0 || value.compare("true") == 0) {
            return VariableType::Boolean;
        }
        return VariableType::String;
    }

    Variable Variable::create(const std::string value) {
        return Variable(value);
    }

    Variable Variable::create_boolean(const bool value) {
        return Variable(std::string(value ? "true" : "false"));
    }

    Variable Variable::create_integer(const int value) {
        return Variable(std::to_string(value));
    }

    Variable Variable::create_float(const float value) {
        return Variable(std::to_string(value));
    }

    Variable Variable::create_string(const std::string value) {
        std::stringstream ss;
        ss << '\"' << value << '\"';
        return Variable(ss.str());
    }

    Variable Variable::create_null() {
        return Variable("null");
    }
    
    void Variable::set(const std::string value) {
        this->value = value;
        this->variable_type = determine_type(value);

        if(value[0] == '\"' && value[value.size() - 1] == '\"') {
            this->string_value = value.substr(1, value.size() - 2);
        }
        else {
            this->string_value = value;
        }

        switch (variable_type)
        {
        case VariableType::Integer:
            integer_value = std::stoi(this->value);
            float_value = static_cast<float>(integer_value);
            boolean_value = false;//not a bool
            break;
        case VariableType::Float:
            float_value = std::stof(this->value);
            integer_value = static_cast<int>(std::roundf(float_value));
            boolean_value = false;//not a bool
            break;
        case VariableType::Boolean:
            boolean_value = this->value.compare("true") == 0;
            integer_value = 0;
            float_value = 0.f;
            break;
        case VariableType::String:
        case VariableType::Null:
            boolean_value = false;//not a bool
            integer_value = 0;
            float_value = 0.f;
            break;        
        default:
            break;
        }
    }

    void Variable::set_boolean(const bool value) {
        set(value ? "true" : "false");
    }

    void Variable::set_integer(const int value) {
        set(std::to_string(value));
    }

    void Variable::set_float(const float value) {
        set(std::to_string(value));
    }

    void Variable::set_string(const std::string value) {
        std::stringstream ss;
        ss << '\"' << value << '\"';
        set(ss.str());
    }

    void Variable::set_null() {
        set("null");
    }


    bool Variable::get_boolean_value() const {
        return boolean_value;
    }

    int Variable::get_integer_value() const {
        return integer_value;
    }

    float Variable::get_float_value() const {
        return float_value;
    }

    std::string Variable::get_string_value() const {
        return string_value;
    }

    std::string Variable::get_raw_value() const {
        return value;
    }

    VariableType Variable::get_type() const {
        return variable_type;
    }

    bool Variable::is_number() const {
        return variable_type == VariableType::Float || variable_type == VariableType::Integer;
    }

    bool Variable::is_null() const {
        return variable_type != VariableType::Null;
    }

    bool Variable::is_equal(const Variable& other) const {
        if(other.is_number()) {
            return float_value == other.float_value;
        }
        else {
            return other.variable_type == variable_type && other.value.compare(value) == 0;
        }
    }
}