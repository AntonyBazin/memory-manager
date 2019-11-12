//
// Created by antony on 10/29/19.
//

#ifndef MEMORY_MANAGER_MANAGER_H
#define MEMORY_MANAGER_MANAGER_H

#include <string>
#include <utility>
#include <vector>
#include <typeinfo>

namespace manager{

    class Table;
    class Program;
    class App;
    class Entity;
    class Array;
    class Link;
    class Value;
    class DivSeg;


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
        explicit Entity(Unit un, std::string nm  = "default_name") :
        position(un),
        name(std::move(nm)) {};

        virtual ~Entity() = default;
    };


    class Program{
    private:
        unsigned int ent_amount;
        Entity* entities;
        std::string file_address;
        const unsigned int memory_quota;
    public:
        Program() :
        ent_amount(0),
        entities(nullptr),
        file_address("default"),
        memory_quota(50) {};

        explicit Program(unsigned int t_mem = 50, std::string t_addr = "default") :
        memory_quota(t_mem),
        file_address(std::move(t_addr)),
        entities(nullptr),
        ent_amount(0) {};

        void request_memory(unsigned int t_amout, Entity* ptr);

        void refuse_divseg(Entity*);
        void free_entity(unsigned int t_index);
        void free_all_memory();

        unsigned int memory_used() const;

        std::ostream& show_all(std::ostream&);
        std::ostream& show_divsegs(std::ostream&);

        ~Program() { delete [] entities; };
    };


    class Table{
    private:
        static const int max_size = 300;
        unsigned int current_size;
        unsigned char *memory;
        unsigned int free_blocks_count;
        Unit* free_blocks;
        Table();
    public:
        static Table& cmd_Table();   // command table

        void defragmentation();   // obvious

        void mark_free();   // for programs to return memory to heap

        Entity* allocate_memory(unsigned int t_size, std::type_info& info);

        ~Table() { delete [] memory; delete [] free_blocks; };
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
        Entity* create_link();
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


    class Array : protected Entity{
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
        DivSeg(std::vector<int> vec);
        int& operator [](unsigned int t_index);
        int operator [](unsigned int t_index) const;
        void free_program(Program* program);
        void cleanup();  // delete all program links
    };

}

#endif //MEMORY_MANAGER_MANAGER_H
