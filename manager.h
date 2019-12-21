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
     * \brief This abstract class describes and Entity.
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
     * \brief This structure describes the position of a memory block.
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
        Entity_ID e_id;    ///< This field describes the Entity's ID
        std::string name;  ///< This field describes the Entity's name
        Unit position;     ///< This field describes the Entity's data position in the table
        size_t refs;       ///< This field tells the amount of usages of the Entity in the programs
        size_t single_size;///< This field describes the Entity's size(for Value) or the size of 1 element
    public:
        //! A trivial constructor
        Entity() : position({}),
                   e_id(E_ERR),
                   name({}),
                   refs(0),
                   single_size(0) {}
        //! Copying constructor
        Entity(const Entity&);
        //! moving constructor
        Entity(Entity&&) noexcept;

        /*!
         * \brief A pure virtual clone method.
         * \return A clone of this class object
         * \sa create_link(std::string), show(const Table&, std::ostream&), and run(Table&, std::ostream&)
         */
        virtual Entity* clone() const = 0;

        /*!
         * \brief A pure virtual method for creating links.
         * \return A link to this Entity
         * \sa clone(), show(const Table&, std::ostream&), and run(Table&, std::ostream&)
         */
        virtual Entity* create_link(std::string) const = 0;

        /*!
         * \brief A pure virtual show method.
         * \sa create_link(std::string), clone(), and run(Table&, std::ostream&)
         */
        virtual std::ostream& show(const Table&, std::ostream&) const = 0;

        /*!
         * \brief A pure virtual run method.
         * The method is used to interact with the user
         * \sa create_link(std::string), show(const Table&, std::ostream&), and clone()
         */
        virtual std::ostream& run(Table&, std::ostream&) = 0;

        /*!
         * \brief A method to set the Entity's ID.
         * \sa e_id
         */
        void set_id(Entity_ID id) noexcept { e_id = id; }

        /*!
         * \brief A method to set the Entity's data position.
         * \sa position
         */
        void set_pos(Unit un) noexcept { position = un; }

        /*!
         * \brief A method to set the Entity's name.
         */
        void set_name(const std::string& t_name) noexcept { name = t_name; }

        /*!
         * \brief A method to set the Entity's single_size.
         * \sa single_size
         */
        void set_single_size(size_t sz) { single_size = sz; }

        /*!
         * \brief A method to get the Entity's position.
         */
        Unit get_pos() const noexcept { return position; }

        /*!
         * \brief A method to get the Entity's single_size.
         * \sa single_size
         */
        size_t get_single_size() const noexcept { return single_size; }

        /*!
         * \brief A method to get the Entity's size in the memory.
         * \sa Unit
         */
        size_t get_size() const noexcept { return  position.size; }

        /*!
         * \brief A method to get the Entity's ID.
         */
        Entity_ID get_entity_id() const noexcept { return e_id; }

        /*!
         * \brief A method to get the Entity's name.
         */
        std::string get_name() const noexcept { return name; }

        /*!
         * \brief A method to get the Entity's refs count.
         */
        size_t get_refs_count() const noexcept { return refs; }


        /*!
         * \brief A method to increment the references counter for the Entity.
         * \sa refs
         */
        void increment_refs() noexcept { ++refs; }
        /*!
         * \brief A method to decrement the references counter for the Entity.
         * \sa refs
         */
        void decrement_refs() noexcept { --refs; }

        /*!
         * \brief A static fabric method to create Entities.
         * \param e_id the ID of the Entity
         * \param single_size the size of 1 element(for arrays) or of the value(for single values)
         * \param t_name The name to be iven to the Entity
         */
        static Entity* generate_Entity(Entity_ID e_id,
                size_t single_size,
                const std::string& t_name = "def") noexcept(false);

        //! \brief Just a virtual default destructor.
        virtual ~Entity() = default;
    };



    class Table{
    private:
        static const int max_size = 500;    ///< This field describes the Table's memory maximum size
        std::vector<unsigned char> memory;  ///< This vector contains the actual memory of the system
        std::vector<Unit> free_blocks;      ///< This vector contains descriptions of free blocks in memory
    public:
        //! A trivial constructor
        Table();

        /*!
         * \brief A method to defragment the system's memory in case of memory shortage.
         * \sa free_blocks
         */
        void defragmentation();

        /*!
         * \brief A method to mark a block of memory as free and available for allocation.
         * \param t_strt the starter address of memory to free
         * \param t_size the size of memory to free
         * \sa free_blocks
         */
        void mark_free(size_t t_strt, size_t t_size) noexcept(false);

        /*!
         * \brief A method to allocate memory from the table.
         * \param t_size the requested size
         * \return a Unit describing the allocated memory position
         * \sa Entity, Unit
         */
        Unit allocate_memory(size_t t_size);

        /*!
         * \brief A method to read bytes from the table.
         * \param t_strt the address to begin reading at
         * \param t_size the size to read
         * \return a vector of bytes read from the table
         * \sa memory, Entity
         */
        std::vector<unsigned char> read_bytes(size_t t_strt,
                size_t t_size) const noexcept(false);

        /*!
         * \brief A method to write something to the system's memory.
         * \param t_strt the address to start writing to
         * \param t_size the size of the block to writa to
         * \param t_vec a vector of bytes to write to the memory
         * \sa memory, Entity
         */
        void write(size_t t_strt,
                size_t t_size,
                std::vector<unsigned char>t_vec) noexcept(false);

        //! \brief A trivial destructor
        ~Table() = default;
    };



    class Program{
    private:
        std::vector<Entity*> entities;   ///< the entities the program can operate with
        std::string file_address;        ///< file address string
        const size_t memory_quota;       ///< max amount of memory available to this program
        Table* table;                    ///< a program has no meaning w/o a table to store data in
        static const int menus;          ///< menus amount
        static std::string menu[];       ///< menus

        /*!
         * \brief A method to calculate the total amount of memory used by this Program.
         * \return the total memory size in bytes used by all Entities in this Program
         * \sa Entity, entities, Table
         */
        size_t memory_used() const;

        /*!
         * \brief A method to request the memory from the Table.
         * \param t_amount the amount of blocks needed
         * \param single_val the size of one block
         * \param e_id the id of the Entity to be created
         * \param t_name the name of the Entity to be created
         * \return a pointer to the created Entity object
         * \sa Entity, Table, Value, Array, DivSeg
         */
        Entity* request_memory(size_t t_amount,
                size_t single_val,
                Entity_ID e_id,
                const std::string& t_name) noexcept(false);

        /*!
         * \brief A method to add an Entity to the Program.
         * \param ent the Entity to be added
         * \sa Entity, Value, Array, Link, DivSeg
         */
        void add_entity(Entity* ent) noexcept(false);

        /*!
         * \brief A method to free an Entity.
         * \param t_index the index of the Entity to be freed
         * \note Support the DivSeg and references counter logic
         * \sa Entity, DivSeg
         */
        void free_entity(size_t t_index) noexcept(false);

        /*!
         * \brief A method to check the Entities for invalid Links.
         * \param guard the position of the value to check the Links for
         * \sa Entity, Unit
         */
        void check_links(Unit guard);

        //! \brief A method to free all memory used by this Program.
        void free_all_memory() noexcept;

        /*!
         * \brief A method to show all info about all Entities of the Program.
         * \sa Entity
         */
        std::ostream& show_all(std::ostream&) const noexcept;

        //! \brief A dialogue method to create an Entity.
        int d_create_entity();

        //! \brief A dialogue method to free an Entity.
        int d_free_memory();

        //! \brief A dialogue method to interact with an Entity.
        int d_use_entity();

        //! \brief A dialogue method to show all memory usage details.
        int d_show_all();

        //! \brief A dialogue method to show Dividable Segments available.
        int d_show_divsegs();

        //! \brief A dialogue method to print menus and ask what to do.
        int answer(int menus_count, std::string variants[]);

        int (Program::*fptr[6])();  ///< a function pointer to dialogue methods

    public:

        //! \brief The default constructor of the Program has no meaning in this scope.
        Program() = delete;

        /*!
         * \brief A basic constructor of the class.
         * \param table the Table this Program uses
         * \param t_mem the memory quota available for this Program
         * \param t_addr the name of the source file of the Program
         * \sa Table
         */
        explicit Program(Table* table, size_t t_mem, std::string t_addr);

        //! A copying '=' operator
        Program& operator =(const Program&);

        //! A moving '=' operator
        Program& operator =(Program&&) noexcept;

        //! A '==' operator used to compare Program's equality
        bool operator ==(const Program&);

        /*!
         * \brief A method to get all Dividable Segments available.
         * \return A vector of the added DivSegs
         * \sa Entity, DivSeg
         */
        std::vector<Entity*> get_div_segs() noexcept;

        /*!
         * \brief A method to add existing DivSegs to this Program.
         * \param ent the pointer to the DivSeg to be added
         * \sa Entity, DivSeg
         */
        void add_existing_DivSeg(Entity* ent) noexcept(false);

        //! \brief A dialogue method to run the Program.
        int run();

        //! \brief A method to return the file address of this Program.
        std::string get_address() const noexcept { return file_address; }
        //! \brief A copying constructor .
        Program(const Program&);
        //! \brief The destructor of the Program.
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
