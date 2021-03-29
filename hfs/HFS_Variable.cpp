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
        int hifen_count = 0;
        if(value.size() > 0) {
            if(value[0] == '-') {
                hifen_count++;
            }
        }

        if(value.size() > 0 && (num_count + dot_count + hifen_count) == value.size() && dot_count <= 1 && hifen_count <= 1) {//definetly a number
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

    Variable Variable::create_dictionary(const std::vector<std::string> keys, const std::vector<Variable> values) {
        Variable new_var = Variable("");
        new_var.variable_type = VariableType::Dictionary;
        
        auto itr_k = keys.begin();
        auto itr_v = values.begin();
        for(; itr_k != keys.end() && itr_v != values.end(); itr_k++, itr_v++) {
            new_var.set_dictionary_entry(*itr_k, *itr_v);
        }

        return new_var;
    }

    Variable Variable::create_dictionary(const std::string key, const Variable value) {
        std::vector<std::string> keys =  { key };
        std::vector<Variable> values = { value };
        return Variable::create_dictionary(keys, values);
    }


    Variable Variable::create_null() {
        return Variable("null");
    }

    Variable Variable::create_copy(const Variable& other) {
        Variable v = other;
        return v;
    }
    
    void Variable::set(const std::string value) {
        this->value = value;
        this->variable_type = determine_type(value);
        dictionary.clear();

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
            float_value = 10.f;
            break;
        }
    }

    void Variable::set_boolean(const bool value) {// TODO: make these manually for improved performance
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

    void Variable::set_dictionary_entry(std::string key, Variable value) {
        this->variable_type = VariableType::Dictionary;
            
        this->boolean_value = false;
        this->integer_value = 0;
        this->float_value = 0.0f;
        this->string_value = "";
        this->value = "";


        auto entry = dictionary.find(key);
        if(value.get_type() == VariableType::Null) {//remove or avoid adding  TODO: Set to null does not delete data anymore
            if(entry != dictionary.end()) {
                dictionary.erase(entry);
            }
        }
        else {
            dictionary.insert_or_assign(key, value);
        }
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

    Variable* Variable::get_dictionary_entry(const std::string key) {
        auto itr = dictionary.find(key);
        if(itr != dictionary.end()) {
            return &itr->second;
        }

        return nullptr;
    }

    Variable* Variable::get_or_create_dictionary_entry(const std::string key, const Variable default_value) {
        auto var = get_dictionary_entry(key);
        if(var == nullptr) {
            set_dictionary_entry(key, Variable::create_copy(default_value));
            var = get_dictionary_entry(key);
        }

        return var;
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