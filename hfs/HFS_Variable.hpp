#ifndef HFS_VARIABLE_HPP
#define HFS_VARIABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>

namespace hfs {
    enum class VariableType {
        Null,
        Boolean,
        Integer,
        Float,
        String,
        Dictionary,
        Array,//TODO:
        Byte//TODO:
    };

    class Variable {
    private:
        Variable(const std::string value);

        std::string value = "";//the written value of the variable, which is parsed to determine variable type
        VariableType variable_type = VariableType::Null;

        //variable types for faster return calls
        bool boolean_value = false;
        int integer_value = 0;
        float float_value = 0.f;
        std::string string_value = "";//this exists because str value may be different than value when quotes are used
        std::unordered_map<std::string, Variable> dictionary = std::unordered_map<std::string, Variable>();
        std::vector<Variable> array = std::vector<Variable>();
    public:
        static VariableType determine_type(const std::string value);

        static Variable       create(const std::string value);
        static Variable       create_boolean(const bool value);
        static Variable       create_integer(const int value);
        static Variable       create_float(const float value);
        static Variable       create_string(const std::string value);
        static Variable       create_dictionary(const std::vector<std::string> keys, const std::vector<Variable> values);
        static Variable       create_dictionary(const std::string key, const Variable value);
        static Variable       create_array(const std::vector<Variable> values);
        static Variable       create_null();
        static Variable       create_copy(const Variable& other);

        void                  set(const std::string value);
        void                  set_boolean(const bool value);
        void                  set_integer(const int value);
        void                  set_float(const float value);
        void                  set_string(const std::string value);
        void                  set_dictionary_entry(std::string key, Variable value);
        void                  set_array_entry(const int index, Variable value);
        void                  set_null();

        bool                  get_boolean_value() const;
        int                   get_integer_value() const;
        float                 get_float_value() const;
        std::string           get_string_value() const;
        std::string           get_raw_value() const;

        Variable*             get_dictionary_entry(const std::string key);
        Variable*             get_or_create_dictionary_entry(const std::string key, const Variable default_value);

        Variable*             get_array_entry(const int index);
        Variable*             get_or_create_array_entry(const int index, const Variable default_value);

        VariableType          get_type() const;

        bool                  is_number() const;
        bool                  is_null() const;
        bool                  is_equal(const Variable& other) const;
    };
}

#endif