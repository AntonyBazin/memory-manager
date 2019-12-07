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
    class Array;
    class Link;
    class Value;
    class DivSeg;

    enum Entity_ID{ Value_ID = 0, Array_ID, DivSeg_ID, Link_ID, E_ERR };
    //enum Type_ID{ CHAR = 0, INT, LONG, LONGLONG, FLOAT, DOUBLE, LONGDOUBLE, T_ERR };


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
        std::string name;
        Unit position;
        size_t refs;
        virtual std::ostream& show(Table&, std::ostream&) const = 0;
    public:
        Entity() : position({}),
                   e_id(E_ERR),
                   name({}),
                   refs(0) {}
        Entity(const Entity&);
        Entity(Entity&&) noexcept;

        virtual Entity* clone() const = 0;
        virtual Entity* create_link(std::string) const = 0;

        void set_pos(Unit un) noexcept { position = un; };
        void set_name(const std::string& t_name) noexcept { name = t_name; }

        Unit get_pos() const noexcept { return position; };
        size_t get_size() const noexcept { return  position.size; };
        Entity_ID get_entity_id() const noexcept { return e_id; }
        const std::string& get_name() const noexcept { return name; }
        size_t get_refs_count() const noexcept { return refs; }
        void increment_refs() noexcept { ++refs; }
        void decrement_refs() noexcept { --refs; }

        static Entity* generate_Entity(Entity_ID e_id,
                const std::string& t_name = "def") noexcept(false);

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
        Table table;  // a program has no meaning w/o a table to store data in
        static const int menus;  // menus amount
        static std::string menu[];  // menus

        size_t memory_used() const;   // calculate memory usage
        Entity* request_memory(size_t t_amount, Entity_ID e_id,
                const std::string& t_name) noexcept(false);
        void add_entity(Entity*);

        void free_entity(size_t t_index) noexcept(false);
        void free_all_memory() noexcept;
        void add_existing_DivSeg(Entity*) noexcept(false);
        void refuse_div_seg(Entity*) noexcept(false);
        std::vector<Entity*> get_div_segs() noexcept;

        std::ostream& show_all(std::ostream&) const;

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



    class Value : public Entity{
    protected:
        std::ostream& show(Table&, std::ostream&) const override;
    public:
        Entity* clone() const override;
        Entity* create_link(std::string) const override;
        Value() = default;
        Value(const Value&) = default;
        Value(Value&&) noexcept;
        long long get_instance(Table&) const;
        void set_instance(Table&, long long new_inst) noexcept(false);
        ~Value() override = default;
    };



    class Link : public Entity{
    private:
        Entity* ptr;
    protected:
        std::ostream& show(Table&, std::ostream&) const override;
    public:
        Link() = delete;
        explicit Link(Entity*, std::string t_name = "link_name");
        Link(const Link&);
        Link(Link&&) noexcept;
        Entity* clone() const override;
        Entity* create_link(std::string) const override;

        long long get_instance(Table&) const;
        void set_instance(Table&, long long new_inst, size_t index = 0);
        Entity* get_core_entity() const;
        ~Link() override = default;
    };



    class Array : public Entity{
    protected:
        std::ostream& show(Table&, std::ostream&) const override;
    public:
        Array() = default;
        Array(const Array&) = default;
        Array(Array&&) noexcept;

        Entity* clone() const override;
        Entity* create_link(std::string) const override;
        long long get_single_instance(Table&, size_t t_begin) const;
        void set_single_instance(Table&, size_t where, long long what);
        std::vector<long long> operator ()(Table&, size_t t_begin, size_t t_end);
        ~Array() override = default;
    };



    class DivSeg : public Array{
    protected:
        std::ostream& show(Table&, std::ostream&) const override;
        std::vector<Program> programs;
    public:
        DivSeg() = default;
        DivSeg(const DivSeg&);
        DivSeg(DivSeg&&) noexcept;

        Entity* clone() const override;
        Entity* create_link(std::string) const override;
        void add_program(Program&);
        void erase_program(Program&);

        void nullify_programs() { this->programs = {}; }
        ~DivSeg() override = default;
    };

}

#endif //MEMORY_MANAGER_MANAGER_H
