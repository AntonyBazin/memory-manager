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
    template<class T>class Array;
    template<class T>class Link;
    template<class T> class Value;
    template<class T>class DivSeg;

    enum Entity_ID{ Value_ID = 0, Array_ID = 1, DivSeg_ID = 2, Link_ID = 3 };
    enum Type_ID{ CHAR = 0, INT, LONG, LONGLONG, FLOAT, DOUBLE, LONGDOUBLE };


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
        static Entity* generate_Entity(Entity_ID id, Type_ID type) noexcept(false);
        template<class T> static Entity* create_Entity(Entity_ID id) noexcept(false);
    };



    class Table{
    private:
        static const int max_size = 300;
        static std::vector<unsigned char> memory;
        std::vector<Unit> free_blocks;

    public:
        Table();
        void defragmentation();   // obvious
        void mark_free(size_t t_strt, size_t t_size) noexcept(false);   // for programs to return memory to heap
        Unit allocate_memory(size_t t_size, Entity_ID id);
        std::vector<unsigned char> read_bytes(size_t t_strt, size_t t_size) noexcept(false);
        void write(size_t t_strt, size_t t_size, std::vector<unsigned char>t_vec) noexcept(false);
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
        template<class T> Entity* request_memory(size_t t_amount, Entity_ID t_id) noexcept(false);
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

        int (Program::*fptr[7])(Table&);
        int answer(int menus_count, std::string menus[]);

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



    template<class T>
    class Value : public Entity{
    protected:
        std::ostream& show(std::ostream&) const override;
    public:
        Value();
        size_t get_size();
        T get_instance(Table&);
        void set_instance(Table&, T new_inst) noexcept(false);
        Entity* create_link() const;
        static Entity* create_Value(Entity_ID id) noexcept(false);
    };



    template<class T>
    class Link : public Entity{
    private:
        Entity* ptr;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        explicit Link(const Entity*);
        T get_instance(Table&);
        void set_instance(Table&, T new_inst);
        Entity* get_core_entity();
        static Entity* create_Link(Entity_ID id) noexcept(false);
    };



    template<class T>
    class Array : public Entity{
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        Array();
        int& operator [](size_t t_index);
        int operator [](size_t t_index) const;
        std::vector<T> operator ()(Table&, size_t t_begin, size_t t_end);
        void set_array(Table&, std::vector<T>);
        static Entity* create_Array(Entity_ID id) noexcept(false);
    };



    template<class T>
    class DivSeg : public Entity{
    private:
        std::vector<Program*> programs;
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        DivSeg();
        void free_program(Program*);
        void erase_one(Program*);
        void cleanup();  // delete all program links
        static Entity* create_DivSeg(Entity_ID id) noexcept(false);
    };

}

#endif //MEMORY_MANAGER_MANAGER_H
