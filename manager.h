//
// Created by antony on 10/29/19.
//

#ifndef MEMORY_MANAGER_MANAGER_H
#define MEMORY_MANAGER_MANAGER_H

#include <algorithm>
#include <vector>
#include <string>
#include <stdexcept>
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
        size_t starter_address;
        size_t size;
        bool active;

        Unit(size_t t_strt, size_t t_size, bool t_actv) :
        starter_address(t_strt), size(t_size), active(t_actv){};

        Unit() : starter_address(0), size(0), active(false) {};
    };


    class Entity{
    protected:
        Entity_ID id;
        std::string name;
        Unit position;
        virtual std::ostream& show(std::ostream&) const = 0;
    public:
        void set_pos(Unit un);
        Unit get_pos() const;
        explicit Entity(Unit un, Entity_ID t_id, std::string nm  = "default_name") :
        position(un),
        id(t_id),
        name(std::move(nm)) {};
        size_t memory_used() const;
        virtual ~Entity() = default;
        Entity_ID get_id() const { return id; }
        static Entity* create_Entity(Entity_ID id) noexcept(false);
    };



    class Table{
    private:
        static const int max_size = 300;
        size_t current_size;
        static std::vector<unsigned char> memory;
        std::vector<Unit> free_blocks;

    public:
        Table();
        void defragmentation();   // obvious
        void mark_free(size_t t_strt, size_t t_size) noexcept(false);   // for programs to return memory to heap
        Unit allocate_memory(size_t t_size, Entity_ID id);
        std::vector<unsigned char> read_bytes(size_t t_strt, size_t t_size) noexcept(false);
        ~Table() = default;
    };



    class Program{
    private:
        std::vector<Entity*> entities; // the entities the program can operate with
        std::string file_address;    // file address
        const size_t memory_quota;   // max amount of memory available to this program
        Table table;  // a program has no meaning w/o a table to store data in

        static const int menus = 8;
        static std::string menu[menus];

        size_t get_memory_used() const;   // calculate memory usage
        Entity* request_memory(size_t t_amount, Entity_ID t_id) noexcept(false);
        void refuse_divseg(Entity*) noexcept(false);
        void free_entity(size_t t_index) noexcept(false);
        void free_all_memory() noexcept;
        std::ostream& show_all(std::ostream&) const;
        std::ostream& show_divsegs(std::ostream&) const;

        int d_request_memory(Table&);
        int d_free_memory(Table&);
        int d_use_divsegs(Table&);
        int d_show_all(Table&);
        int d_show_divsegs(Table&);
        int d_calc_memory(Table&);

        int answer(std::string alternatives[], int n);
        int (Program::*fptr[7])(Table&);

    public:
        Program() = delete;
        explicit Program(Table& table, size_t t_mem = 50, std::string t_addr = "default");
        int run();
        Program clone();
        Program(const Program&);
        Program(Program&&) noexcept;
        ~Program();
    };



    class App{
    private:
        size_t programs_count;
        Program* active_programs;
    public:
        std::ostream& show_all(std::ostream&);
        std::ostream& print_prog_memory(std::ostream&, size_t t_index);
        std::ostream& show_errors(std::ostream&);
        std::ostream& show_incorrect_links(std::ostream&);
        void command(std::iostream&);  // for menus
    };


    class Value : public Entity{
    protected:
        std::ostream& show(std::ostream&) const override;
    public:
        Value();
        Value(int val);
        unsigned int get_instance(Table&);
        int set_instance(Table&, int t_new_inst);
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
        size_t memory_used;
        size_t element_size;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        Array();
        Array(std::vector<int> vec);
        int& operator [](size_t t_index);
        int operator [](size_t t_index) const;
        std::vector<int> operator ()(size_t t_begin, size_t t_end);
        friend std::ostream& operator << (std::ostream&, const Array);
    };


    class DivSeg : public Entity{
    private:
        size_t memory_used;
        size_t element_size;
        std::vector<Program*> programs;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        DivSeg();
        DivSeg(std::vector<int> vec);
        int& operator [](size_t t_index);
        int operator [](size_t t_index) const;
        void free_program(Program*);
        void erase_one(Program*);
        void cleanup();  // delete all program links
    };

}

#endif //MEMORY_MANAGER_MANAGER_H
