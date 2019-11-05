//
// Created by antony on 10/29/19.
//

#ifndef MEMORY_MANAGER_MANAGER_H
#define MEMORY_MANAGER_MANAGER_H

namespace manager{
    class Table;
    class Program;
    class Entity;
    class Array;
    class Link;
    class Value;
    class DivSeg;

    class Table{
    private:
        static const int max_size = 200;
        unsigned int current_size;
        unsigned int used_units_count;
        Program* programs;
        unsigned int programs_amount;
        int *memory;
        Table();
    public:
        Table& cmd_Table();
    };

    class

}


#endif //MEMORY_MANAGER_MANAGER_H
