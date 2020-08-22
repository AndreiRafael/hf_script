#include <iostream>

#include "HFS_Scope.hpp"
#include "basic/HFS_Math.hpp"
#include "basic/HFS_Essentials.hpp"
#include "operations/HFS_Operations.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Hello World!";

    hfs::Scope main_scope;

    hfs::Scope child_scope(&main_scope);

    hfs::Operation* value = new hfs::RawValueOperation("10");
    hfs::Operation* set = new hfs::SetOperation("lol", value);

    auto v = set->run(&main_scope);

    return 0;
}