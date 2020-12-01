#include <iostream>

#include "HFS_Scope.hpp"
#include "basic/HFS_Math.hpp"
#include "basic/HFS_Essentials.hpp"
#include "operations/HFS_Operations.hpp"
#include "HFS_ScriptRunner.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Hello World!";

    hfs::Scope main_scope;
    hfs::Scope child_scope(&main_scope);

    hfs::Operation* value = new hfs::RawValueOperation("10");
    hfs::Operation* set = new hfs::SetOperation("lol", value);

    hfs::ScriptRunner runner;
    std::string error_string;
    runner.load_script("C:/Users/andre/OneDrive/Documentos/script.hfs", &error_string);

    //auto v = set->run(&main_scope);

    return 0;
}