Hifire Script
=====
*Hifire Script* is a simple scripting language which operates by binding c++ functions and has coroutine support.

### Language Structure

Code is made out of functions, which may be called by regular c++ code. Functions may operate on different variables, local or global.
Variables can assume 1 of 5 types: integer, float, boolean, string or dictionary, the latter being a collection of other variables identified via keys. A variable will change types during its lifetime if a difefrent type is assigned to it.  
Variable are not 

```
func my_function() {
    my_int = 10;
    my_float = 10.0;
    my_bool = true;
    my_string = "Hello World!";

    my_dictionary["score"] = my_int;#creates variable *my_dictionary*, of type dictionary and field *score*
    my_dictionary["text"] = my_string;#adds field *text* to existing variable *my_dictionary*
}
```