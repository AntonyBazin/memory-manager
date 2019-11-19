//
// Created by antony on 10/29/19.
//

#ifndef MEMORY_MANAGER_MANAGER_H
#define MEMORY_MANAGER_MANAGER_H

#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <typeinfo>
#include <iostream>

namespace manager{

    class Table;
    class Program;
    class App;
    class Entity;
    class Array;
    class Link;
    class Value;
    class DivSeg;


    enum Entity_ID{ Value_ID = 0, Array_ID = 1, DivSeg_ID = 2 };


    struct Unit{
        unsigned int starter_address;
        unsigned int size;
        bool active;

        Unit(unsigned int t_strt, unsigned int t_size, bool t_actv) :
        starter_address(t_strt), size(t_size), active(t_actv){};

        Unit() : starter_address(0), size(0), active(false) {};
    };


    class Entity{
    protected:
        std::string name;
        Unit position;
        virtual std::ostream& show(std::ostream&) const = 0;
    public:
        void set_pos(Unit un);
        explicit Entity(Unit un, std::string nm  = "default_name") :
        position(un),
        name(std::move(nm)) {};

        virtual ~Entity() = default;

        static Entity* create_Entity(Entity_ID id) noexcept(false);
    };


    class Program{
    private:
        std::vector<Entity*> entities;
        std::string file_address;
        const unsigned int memory_quota;
        unsigned int get_memory_used() const;

        static const int menus = 7;
        static std::string menu[menus];

        int request_memory(unsigned int t_amout, Entity* ptr);
        void refuse_divseg(Entity*);
        void free_entity(unsigned int t_index);
        void free_all_memory();
        std::ostream& show_all(std::ostream&) const;
        std::ostream& show_divsegs(std::ostream&) const;

        int d_request_memory();
        int d_free_memory();
        int d_use_divsegs();
        int d_show_all();
        int d_show_divsegs();
        int d_calc_memory();

        int answer(std::iostream&, std::string alternatives[], int n);

        int (Program::*fptr[7])();
    public:
        Program();

        explicit Program(unsigned int t_mem = 50, std::string t_addr = "default");

        int run(std::iostream&);

        ~Program();
    };


    class Table{
    private:
        static const int max_size = 300;
        unsigned int current_size;
        static std::vector<unsigned char> memory;
        std::vector<Unit> free_blocks;
    public:
        Table();

        void defragmentation();   // obvious

        void mark_free(unsigned int t_strt, unsigned int t_size) noexcept(false);   // for programs to return memory to heap

        Entity* allocate_memory(unsigned int t_size, Entity_ID id);

        ~Table() = default;
    };


    class App{
    private:
        unsigned int programs_count;
        Program* active_programs;
    public:
        std::ostream& show_all(std::ostream&);
        std::ostream& print_prog_memory(std::ostream&, unsigned int t_index);
        std::ostream& show_errors(std::ostream&);
        std::ostream& show_incorrect_links(std::ostream&);
        void command(std::iostream&);  // for menus
    };


    class Value : public Entity{
    private:
        unsigned int memory_used;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        Value();
        Value(int val);
        int get_instance();
        int set_instance(int t_new_inst);
        Entity* create_link();
    };


    class Link : public Entity{
    private:
        Entity* ptr;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        Link(Entity *ptr);
        int get_instance();
        int set_instance(int t_new_inst);
    };


    class Array : public Entity{
    private:
        unsigned int memory_used;
        unsigned int element_size;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        Array();
        Array(std::vector<int> vec);
        int& operator [](unsigned int t_index);
        int operator [](unsigned int t_index) const;
        std::vector<int> operator ()(unsigned int t_begin, unsigned int t_end);
        friend std::ostream& operator << (std::ostream&, const Array);
    };


    class DivSeg : public Entity{
    private:
        unsigned int memory_used;
        unsigned int element_size;
        unsigned int amount_of_programs;
        Program* programs;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        DivSeg();
        DivSeg(std::vector<int> vec);
        int& operator [](unsigned int t_index);
        int operator [](unsigned int t_index) const;
        void free_program(Program* program);
        void cleanup();  // delete all program links
    };

}

#endif //MEMORY_MANAGER_MANAGER_H
