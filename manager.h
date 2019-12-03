//
// Created by antony on 10/29/19.
//

#ifndef MEMORY_MANAGER_MANAGER_H
#define MEMORY_MANAGER_MANAGER_H

#include <algorithm>
#include <utility>  // for std::move to move objects such as string's
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

    enum Entity_ID{ Value_ID = 0, Array_ID, DivSeg_ID, Link_ID, E_ERR };
    enum Type_ID{ CHAR = 0, INT, LONG, LONGLONG, FLOAT, DOUBLE, LONGDOUBLE, T_ERR };


    struct Unit{
        size_t starter_address;
        size_t size;

        Unit(size_t t_strt, size_t t_size) :
        starter_address(t_strt), size(t_size){};

        Unit() : starter_address(0), size(0) {};
    };



    class Entity{
    protected:
        Entity_ID e_id;
        Type_ID t_id;
        std::string name;
        Unit position;
        size_t refs;
        virtual std::ostream& show(std::ostream&) const = 0;
    public:
        void set_pos(Unit un) noexcept { position = un; };
        void set_name(std::string t_name) noexcept { name = std::move(t_name); }
        Unit get_pos() const noexcept;
        /*explicit Entity(Unit un, Entity_ID ent_id,
                Type_ID type_id, std::string nm  = "default_name") :
                position(un),
                e_id(ent_id),
                t_id(type_id),
                name(std::move(nm)),
                refs(0) {};*/

        Entity() : position({}),
                   e_id(E_ERR),
                   t_id(T_ERR),
                   name({}),
                   refs(0) {}

        size_t memory_used() const noexcept;
        Entity_ID get_entity_id() const { return e_id; }
        Type_ID get_type_id() const { return t_id; }
        const std::string& get_name() const { return name; }
        size_t refs_count() const noexcept { return refs; }
        void increment_refs() noexcept { ++refs; }
        void decrement_refs() noexcept { --refs; }

        static Entity* generate_Entity(Entity_ID e_id,
                Type_ID type, const std::string& t_name = "def") noexcept(false);

        template<class T> static Entity* create_Entity(Entity_ID id,
                std::string t_name = "def") noexcept(false);

        Entity* create_link() const;
        virtual ~Entity() = default;
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
        static const int menus;  // menus amount
        static std::string menu[];  // menus

        size_t get_memory_used() const;   // calculate memory usage
        Entity* request_memory(size_t t_amount, Type_ID t_id, Entity_ID e_id,
                const std::string& t_name) noexcept(false);

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
        int answer(int menus_count, std::string menus[]);
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
        std::vector<Program> programs;
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
        Value() = default;
        size_t get_size();
        T get_instance(Table&);
        void set_instance(Table&, T new_inst) noexcept(false);
        ~Value() override = default;
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
    };



    template<class T>
    class Array : public Entity{
    protected:
        std::ostream& show(std::ostream&) const;
    public:
        Array() = default;
        int& operator [](size_t t_index);
        int operator [](size_t t_index) const;
        std::vector<T> operator ()(Table&, size_t t_begin, size_t t_end);
        void set_array(Table&, std::vector<T>);
        ~Array() override = default;
    };



    template<class T>
    class DivSeg : public Entity{
    private:
        std::vector<Program> programs;
    protected:
        std::ostream& show(std::ostream&) const override;
    public:
        DivSeg() = default;
        void cleanup() { refs = 0; }
        ~DivSeg() override = default;
    };

}

#endif //MEMORY_MANAGER_MANAGER_H
