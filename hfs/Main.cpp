#include "HFS.hpp"

int main() {
    hfs::ScriptCompiler compiler;
    auto arr = compiler.tokenize_from_file("D:/Users/andre/source/repos/DD_locuradas/OpenGL/bin/win32/x86/test_script.hfs");
    std::cout << "LOL" << '\n';
    system("pause");
    

    return 0;
}