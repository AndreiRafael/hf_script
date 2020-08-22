#ifndef HFS_VARIABLE_HPP
#define HFS_VARIABLE_HPP

#include <string>

namespace hfs {
    enum class VariableType {
        Null,
        Boolean,
        Integer,
        Float,
        String
    };

    class Variable {
    private:
        Variable(const std::string value);

        std::string value;//the written value of the variable
        VariableType variable_type;

        //variable types for faster return calls
        bool boolean_value = false;
        int integer_value = 0;
        float float_value = 0.f;
        std::string string_value = "";//this exists because str value may be different than value when quotes are used
    public:

        static VariableType determine_type(const std::string value);

        static Variable     create(const std::string value);
        static Variable     create_boolean(const bool value);
        static Variable     create_integer(const int value);
        static Variable     create_float(const float value);
        static Variable     create_string(const std::string value);
        static Variable     create_null();

        void                set(const std::string value);
        void                set_boolean(const bool value);
        void                set_integer(const int value);
        void                set_float(const float value);
        void                set_string(const std::string value);
        void                set_null();

        bool                get_boolean_value() const;
        int                 get_integer_value() const;
        float               get_float_value() const;
        std::string         get_string_value() const;
        std::string         get_raw_value() const;

        VariableType        get_type() const;
        VariableType        get_id() const;

        bool                is_number() const;
        bool                is_null() const;
        bool                is_equal(const Variable& other) const;
    };
}

#endif