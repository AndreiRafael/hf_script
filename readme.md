Hifire Script
=====
*Hifire Script* is a simple scripting language which operates by binding c++ functions and has coroutine support.

# Overview
### Features

### Language Structure

Code is made out of functions and variables, 

Code is made out of functions, which may be called by regular c++ code. Functions may operate on different variables, local or global.
Variables can assume 1 of 6 types: null, integer, float, boolean, string or dictionary, the latter being a collection of other variables identified via keys. A variable will change types during its lifetime if a difefrent type is assigned to it.  
Variable are not 

```
func my_function() {
    null_value = null;
    my_int = 10;
    my_float = 10.0;
    my_bool = true;
    my_string = "Hello World!";

    my_dictionary["score"] = my_int;#creates variable "my_dictionary", of type dictionary and field "score"
    my_dictionary["text"] = my_string;#adds field "text" to existing variable "my_dictionary"
}
```

# Quick Start/ Usage

To begin using *Hifire Script*, copy the contents to your project folder and include "HFS.hpp", make sure to include all .cpps and .hpps to your source, as the code needs to be compiled with the project.

To load scripts, use the *load_from_file* method in an instance of the *Script* class. In order to actually be ran, a *Script* needs to be added to an instance of *ScriptRunner*, via its method *add_script*.

All scripts held by a *ScriptRunner* can access each other functions. Similarly, all scipts on a *ScriptRunner* can use c++ functions bound by the *bind_function* method of the *ScriptRunner*.

To enter a function, call *start_function* from a *ScriptRunner*. The function won't be ran directly, this occurs when *step* is called on a *ScriptRunner*, which should be called frequently, preferably in a loop each frame of the application.


# Other Languages

Currently, c++ is the only available language, with C and C# being planned to be available. No other languages are curently planned.