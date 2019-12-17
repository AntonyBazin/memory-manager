//
// Created by antony on 10/29/19.
//
/*!
\file
\brief Header file with classes declaration

This file contents the declarations of the
classes used in the application
*/

#ifndef MEMORY_MANAGER_MANAGER_H
#define MEMORY_MANAGER_MANAGER_H

#include <algorithm>
#include <utility>  // for std::move to move objects such as string's, and for std::exception
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>  // for std::pow
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

    enum Entity_ID{ Value_ID = 0, Array_ID, DivSeg_ID, Link_ID, E_ERR };


    struct Unit{
        size_t starter_address;
        size_t size;

        Unit(size_t t_strt, size_t t_size) : starter_address(t_strt), size(t_size){};
        Unit() : starter_address(0), size(0) {};
        bool operator==(const Unit&) const;
    };



    class Entity{
    protected:
        Entity_ID e_id;
        std::string name;
        Unit position;
        size_t refs;
        size_t single_size;
    public:
        Entity() : position({}),
                   e_id(E_ERR),
                   name({}),
                   refs(0),
                   single_size(0) {}
        Entity(const Entity&);
        Entity(Entity&&) noexcept;

        virtual Entity* clone() const = 0;
        virtual Entity* create_link(std::string) const = 0;
        virtual std::ostream& show(const Table&, std::ostream&) const = 0;
        virtual std::ostream& run(Table&, std::ostream&) = 0;

        void set_id(Entity_ID id) noexcept { e_id = id; }
        void set_pos(Unit un) noexcept { position = un; }
        void set_name(const std::string& t_name) noexcept { name = t_name; }
        void set_single_size(size_t sz) { single_size = sz; }

        Unit get_pos() const noexcept { return position; }
        size_t get_single_size() const noexcept { return single_size; }
        size_t get_size() const noexcept { return  position.size; }
        Entity_ID get_entity_id() const noexcept { return e_id; }
        std::string get_name() const noexcept { return name; }
        size_t get_refs_count() const noexcept { return refs; }

        void increment_refs() noexcept { ++refs; }
        void decrement_refs() noexcept { --refs; }

        static Entity* generate_Entity(Entity_ID e_id,
                size_t single_size,
                const std::string& t_name = "def") noexcept(false);

        virtual ~Entity() = default;
    };



    class Table{
    private:
        static const int max_size = 500;
        std::vector<unsigned char> memory;
        std::vector<Unit> free_blocks;
    public:
        Table();
        void defragmentation();   // obvious
        void mark_free(size_t t_strt, size_t t_size) noexcept(false);   // for programs to return memory to heap
        Unit allocate_memory(size_t t_size);

        std::vector<unsigned char> read_bytes(size_t t_strt,
                size_t t_size) const noexcept(false);

        void write(size_t t_strt,
                size_t t_size,
                std::vector<unsigned char>t_vec) noexcept(false);
        ~Table() = default;
    };



    class Program{
    private:
        std::vector<Entity*> entities; // the entities the program can operate with
        std::string file_address;    // file address
        const size_t memory_quota;   // max amount of memory available to this program
        Table* table;  // a program has no meaning w/o a table to store data in
        static const int menus;  // menus amount
        static std::string menu[];  // menus

        size_t memory_used() const;   // calculate memory usage

        Entity* request_memory(size_t t_amount,
                size_t single_val,
                Entity_ID e_id,
                const std::string& t_name) noexcept(false);
        void add_entity(Entity*) noexcept(false);
        void free_entity(size_t t_index) noexcept(false);
        void check_links(Unit);
        void free_all_memory() noexcept;

        std::ostream& show_all(std::ostream&) const noexcept;

        int d_create_entity();
        int d_free_memory();
        int d_use_entity();
        int d_show_all();
        int d_show_divsegs();
        int answer(int menus_count, std::string variants[]);
        int (Program::*fptr[6])();

    public:
        Program() = delete;
        explicit Program(Table* table, size_t t_mem, std::string t_addr);
        Program& operator =(const Program&);
        Program& operator =(Program&&) noexcept;
        bool operator ==(const Program&);
        std::vector<Entity*> get_div_segs() noexcept;
        void add_existing_DivSeg(Entity*) noexcept(false); //includes add_program
        int run();
        std::string get_address() const noexcept { return file_address; }
        Program(const Program&);
        ~Program();
    };



    class App{
    private:
        Table* table;
        std::vector<Program*> programs;
    public:
        App();
        void run();
        void create_program();
        int command();
        void add_ds();
        void list_programs();
        ~App();
    };



    class Value : public Entity{
    public:
        Value() = default;
        Value(const Value&) = default;
        Value(Value&&) noexcept;

        std::ostream& show(const Table&, std::ostream&) const override;
        Entity* clone() const override;
        Entity* create_link(std::string) const override;
        std::ostream& run(Table&, std::ostream&) override;

        unsigned long long get_instance(const Table&) const;
        void set_instance(Table&, unsigned long long new_inst) noexcept(false);
        ~Value() override = default;
    };



    class Link : public Entity{
    private:
        Entity* ptr;
    public:
        Link() = delete;
        explicit Link(Entity*, std::string t_name);
        Link(const Link&);
        Link(Link&&) noexcept;

        std::ostream& show(const Table&, std::ostream&) const override;
        Entity* clone() const override;
        Entity* create_link(std::string) const override;
        std::ostream& run(Table&, std::ostream&) override;

        unsigned long long get_instance(const Table&) const;
        void set_instance(Table&, unsigned long long new_inst, size_t index = 0);
        Entity* get_core_entity() const;
        ~Link() override = default;
    };



    class Array : public Entity{
    public:
        Array() = default;
        Array(const Array&) = default;
        Array(Array&&) noexcept;

        std::ostream& show(const Table&, std::ostream&) const override;
        Entity* clone() const override;
        Entity* create_link(std::string) const override;
        std::ostream& run(Table&, std::ostream&) override;

        unsigned long long get_single_instance(const Table&, size_t t_begin) const noexcept(false);
        void set_single_instance(Table&, size_t where, unsigned long long what) noexcept(false);
        std::vector<unsigned long long> operator ()(const Table&,
                size_t t_begin,
                size_t t_end) noexcept(false);  //todo
        ~Array() override = default;
    };



    class DivSeg : public Array{
    protected:
        std::vector<Program*> programs;
    public:
        DivSeg() = default;
        DivSeg(const DivSeg&);
        DivSeg(DivSeg&&) noexcept;

        std::ostream& show(const Table&, std::ostream&) const override;
        Entity* clone() const override;
        Entity* create_link(std::string) const override;
        std::ostream& run(Table&, std::ostream&) override;

        std::ostream& show_programs (std::ostream&) const;
        void add_program(Program*);
        void erase_program(Program*);

        ~DivSeg() override;
    };

}

#endif //MEMORY_MANAGER_MANAGER_H
