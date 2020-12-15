#include <iostream>

#include "HFS_Scope.hpp"
#include "basic/HFS_Math.hpp"
#include "basic/HFS_Essentials.hpp"
#include "operations/HFS_Operations.hpp"
#include "HFS_ScriptRunner.hpp"
#include "HFS_Script.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Hello World!";

    hfs::Scope main_scope;
    hfs::Scope child_scope(&main_scope);

    hfs::RawValueOperation first_key("first");
    hfs::RawValueOperation second_key("second");
    hfs::RawValueOperation value("10");

    hfs::RawValueOperation value55("55");

    hfs::VariableRetrieveOperation retrieve_op("_dictionary");

    std::vector<hfs::Operation*> keys = { &first_key, &second_key };

    hfs::SetOperation op1("_dictionary", &value, keys);
    hfs::SetOperation op2("bruh", &value, keys);
    op1.set_next_operation(&op2);
    hfs::SetOperation op3("brother", &value55);
    op2.set_next_operation(&op3);
    hfs::SetOperation op4("copy", &retrieve_op);
    op3.set_next_operation(&op4);

    hfs::OperationRunner op_runner;
    op_runner.setup(&op1, &child_scope);

    while(op_runner.step() != hfs::RunnerResult::Return) {}

    hfs::ScriptRunner runner;
    std::string error_string;

    hfs::Script script;
    script.load_from_file("D:/Users/andre/OneDrive/Documentos/script.hfs");

    runner.load_script("C:/Users/andre/OneDrive/Documentos/script.hfs", &error_string);

    //auto v = set->run(&main_scope);

    return 0;
}