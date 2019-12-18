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

    /*!
     * \brief This class is used for storing the information
     * and accessing it.
     *
     * Allows programs to allocate memory, write to it, read it,
     * mark it as free. It also has a special defragmentation function
     * for clearing the borders of deallocated memory parts.
     */
    class Table;

    /*!
     * \brief This class describes a program.
     *
     * The Program class is used to store the Entities descriptions.
     * It allows the user to request the table to allocate memory
     * for Entities, get information about them,
     * work with the existing Entities that this Program has access to,
     * and refuse the Entities by deallocating the memory.
     */
    class Program;

    /*!
     * \brief This class is used to contain and give access to Programs.
     *
     * The App class is used to store the Program class objects
     * and provides dialogue methods to work with them.
     */
    class App;

    /*!
     * \brief This class describes and Entity.
     *
     * The Entity class is the abstract class used to work with
     * Entities. It is a Base class for the descriptors of
     * Value, Array, Link, Dividable Segment. It contains the basic
     * fields and methods(including virtual) used by all these Entities.
     */
    class Entity;

    /*!
     * \brief This class describes a single Value.
     *
     * The Value class is a class derived from the Entity class.
     * It contains the methods for setting the value,
     * reading it from the table and converting it into
     * human-readable numbers.
     */
    class Value;

    /*!
     * \brief This class describes an array of values.
     *
     * The Array class is a class derived from the Entity class.
     * It contains the methods for setting the values of array
     * elements, reading them from the table(all at one or in
     * an index range) and converting them into
     * human-readable numbers.
     * \warning This array can only be used inside the program it
     * was created in.
     */
    class Array;

    /*!
     * \brief This class describes a single Link.
     *
     * The Link class is a class derived from the Entity class.
     * It points to some other class, such as Value, Array or
     * Dividable Segment. The user gains access to these Entities
     * through this class.
     * \note This Link can only be used in the program it
     * was created in.
     * \note Since the Link can only point to another Entity,
     * it cannot exist alone, by itself.
     * \warning When the Entity which a Link refers to is deallocated,
     * a message about this incorrect Link will appear,
     * and the Link will be destroyed as well.
     */
    class Link;

    /*!
     * \brief This class describes a Dividable Segment.
     *
     * The Dividable Segment class is a class derived from the Entity class.
     * It contains the methods for setting the values of array
     * elements, reading them from the table(all at one or in
     * an index range) and converting them into
     * human-readable numbers. This type of Array can be used
     * in multiple Programs.
     * \note If this Entity is divided between two or
     * more Programs at the same time, and one of them
     * is ordered to free this Segment, it will only stop working with
     * it, the Entity itself will not be destroyed.
     */
    class DivSeg;

    /// The keys used to identify the Entities
    enum Entity_ID{ Value_ID = 0,  ///< Defines the Entity as a Single Value
            Array_ID,              ///< Defines the Entity as an Array
            DivSeg_ID,             ///< Defines the Entity as a Dividable Segment
            Link_ID,               ///< Defines the Entity as a Link
            E_ERR };               ///< Used in undefined Entites. Will never appear normally.


    /*!
     * \brief This class describes the position of a memory block.
     *
     * It is used by the Table class to store the free blocks
     * and the Entities descriptors to mark the memory
     * available for them.
     */
    struct Unit{
        size_t starter_address;      ///> The start address of the block
        size_t size;                 ///> The size of the block

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
