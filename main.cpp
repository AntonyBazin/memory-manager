#include <iostream>
#include "manager.h"

using namespace manager;

int main() {
    Table table;
    Program pr(table, 300, "file.file");
    pr.run();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}