#include <iostream>
#include <string>

#include "HFS_Scope.hpp"
#include "basic/HFS_Math.hpp"
#include "basic/HFS_Essentials.hpp"
#include "operations/HFS_Operations.hpp"
#include "HFS_ScriptRunner.hpp"
#include "HFS_Script.hpp"

hfs::Variable print(std::vector<hfs::Variable> values) {
    for(int i = 0; i < values.size(); ++i) {
        auto v = values[i];
        switch (v.get_type())
        {
        case hfs::VariableType::Boolean:
        case hfs::VariableType::Null:
            std::cout << v.get_raw_value();
            break;
        case hfs::VariableType::String:
            std::cout << v.get_string_value();
            break;
        case hfs::VariableType::Float:
            std::cout << v.get_float_value();
            break;
        case hfs::VariableType::Integer:
            std::cout << v.get_integer_value();
            break;        
        default:
            std::cout << "{ dictionary }";
        }
    }
    std::cout << '\n';
    return hfs::Variable::create_null();
}

hfs::Variable read(std::vector<hfs::Variable> values) {
    std::string line;
    std::getline(std::cin, line);
    return hfs::Variable::create(line);
}

int main(int argc, char* argv[]) {
    hfs::ScriptRunner runner;
    std::string error_string;

    hfs::Script script;
    if(!script.load_from_file("D:/Users/andre/OneDrive/Documentos/script.hfs")) {
        std::cout << "Script error: " << script.get_error() << '\n';
    }

    runner.add_script(&script, &error_string);
    std::function<hfs::Variable(std::vector<hfs::Variable>)> p = print;
    std::function<hfs::Variable(std::vector<hfs::Variable>)> l = hfs::less;
    std::function<hfs::Variable(std::vector<hfs::Variable>)> s = hfs::sum;
    std::function<hfs::Variable(std::vector<hfs::Variable>)> r = read;
    std::function<hfs::Variable(std::vector<hfs::Variable>)> g = hfs::greater;

    runner.bind_function("print", p);
    runner.bind_function("less", l, 2);
    runner.bind_function("greater", g, 2);
    runner.bind_function("sum", s);
    runner.bind_function("read", r, 0);

    runner.start_function("main", std::vector<hfs::Variable> ());
    while(true) {
        runner.step();
    }

    //auto v = set->run(&main_scope);

    return 0;
}