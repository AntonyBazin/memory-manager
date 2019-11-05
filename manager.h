//
// Created by antony on 10/29/19.
//

#ifndef MEMORY_MANAGER_MANAGER_H
#define MEMORY_MANAGER_MANAGER_H

#include <string>
#include <vector>

namespace manager{
    class Table;
    class Program;
    class Entity;
    class Array;
    class Link;
    class Value;
    class DivSeg;


    struct Unit{
        unsigned int starter_address;
        unsigned int size;
        unsigned int ptr_count;
    };


    class Table{
    private:
        static const int max_size = 300;
        unsigned int current_size;
        unsigned int used_units_count;
        Program* programs;
        unsigned int programs_amount;
        char *memory;
        Table();
    public:
        Table& cmd_Table();

        ~Table() { delete [] memory; };
    };


    class Program{
    private:
        unsigned int ent_amount;
        Entity* entities;
        std::string file_address;
        unsigned int memory_quota;
    public:

    };


    class Entity{
    protected:
        std::string name;
        Unit position;
        virtual std::ostream& show(std::ostream&) const = 0;
    public:
        Entity();
        Entity(Unit);
    };


    class Array : protected Entity{
    private:
        unsigned int memory_used;
        unsigned int element_size;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        Array();
        Array(std::vector<int>);
        int& operator [](unsigned int t_index);
        int operator [](unsigned int t_index) const;
        std::vector<int> operator ()(unsigned int t_begin, unsigned int t_end);
        friend std::ostream& operator << (std::ostream&, const Array);
    };

    class Link : protected Entity{
    private:
        Entity* ptr;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        Link(Entity *ptr);
        int get_instance();
        int set_instance(int t_new_inst);
    };

    class Value : protected Entity{
    private:
        unsigned int memory_used;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        Value();
        Value(int val);
        int get_instance();
        int set_instance(int t_new_inst);
        Link* create_link();
    };

    class DivSeg : protected Entity{
    private:
        unsigned int memory_used;
        unsigned int element_size;
        unsigned int amount_of_programs;
        Program* programs;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        DivSeg();
        DivSeg(std::vector<int>);
        int& operator [](unsigned int t_index);
        int operator [](unsigned int t_index) const;
        void free_program(Program* program);
    };

}


#endif //MEMORY_MANAGER_MANAGER_H
