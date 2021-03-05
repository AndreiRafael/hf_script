This Readme and code itself are in construction and somewhat incomplete

Hifire Script
=====
*Hifire Script* is a simple scripting language which operates by binding c++ functions and has coroutine support.

# Overview
### Features

### Language Structure

Code is made out of functions, which may be called by regular c++ code. Functions may access and modify different variables, local or global.
Variables can assume 1 of 6 types: null, integer, float, boolean, string or dictionary, the latter being a collection of other variables identified by keys. A variable will change types during its lifetime if a different type is assigned to it.

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

To begin using *Hifire Script*, link against *hfs.lib(Windows)* and include "HFS.hpp", then you should be good to go.

To load scripts, use the *load_from_file* method in an instance of the *Script* class. In order to actually be ran, a *Script* needs to be added to an instance of *ScriptHolder*, via its method *add_script*.

All scripts held by a *ScriptHolder* can access each other functions. Similarly, all scipts on a *ScriptHolder* can use c++ functions bound by the *bind_function* method of the *ScriptHolder*.

To enter a function, call *start_function* from a *ScriptHolder*. The function won't be ran directly, this occurs when *step* is called on a *ScriptHolder*, which should be called frequently, preferably in a loop each frame of the application.


# Other Languages

Currently, c++ is the only available language, with C and C# being planned to be available. No other languages are curently planned.
