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
#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>



namespace manager{


    class Table;
    class Program;
    class App;
    class Entity;
    class Value;
    class Array;
    class Link;
    class DivSeg;

    /// The keys used to identify the Entities
    enum Entity_ID{ Value_ID = 0,  ///< Defines the Entity as a Single Value
            Array_ID,              ///< Defines the Entity as an Array
            DivSeg_ID,             ///< Defines the Entity as a Dividable Segment
            Link_ID,               ///< Defines the Entity as a Link
            E_ERR };               ///< Used in undefined Entities. Will never appear normally.


    /*!
     * \brief This structure describes the position of a memory block.
     *
     * It is used by the Table class to store the free blocks
     * and the Entities descriptors to mark the memory
     * available for them.
     */
    struct Unit{
        size_t starter_address;      ///< The start address of the block
        size_t size;                 ///< The size of the block

        //! \brief The Unit constructor initializing the starter_address and size fields
        Unit(size_t t_strt, size_t t_size) : starter_address(t_strt), size(t_size){};

        //! \brief The default Unit constructor
        Unit() : starter_address(0), size(0) {};
        bool operator==(const Unit&) const;
    };



    /*!
     * \brief This abstract class describes an Entity.
     *
     * The Entity class is the abstract class used to work with
     * Entities. It is a Base class for the descriptors of
     * Value, Array, Link, Dividable Segment. It contains the basic
     * fields and methods(including virtual) used by all these Entities.
     */
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



    /*!
     * \brief This class is used for storing the information
     * and accessing it.
     *
     * Allows programs to allocate memory, write to it, read it,
     * mark it as free. It also has a special defragmentation function
     * for clearing the borders of deallocated memory parts.
     */
    class Table{
    private:
        static const int max_size = 500;    ///< This field describes the Table's memory maximum size
        std::vector<unsigned char> memory;  ///< This vector contains the actual memory of the system
        std::vector<Unit> free_blocks;      ///< This vector contains descriptions of free blocks in memory
        std::mutex mtx;                     ///< The mutex object protecting from multitasking errors
        std::condition_variable not_empty;  ///< A condition variable signalizing the table can be written to
        std::condition_variable not_full;   ///< A condition variable signalizing the table can be read from
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
         * \param t_size the size of the block to write to
         * \param t_vec a vector of bytes to write to the memory
         * \sa memory, Entity
         */
        void write(size_t t_strt,
                size_t t_size,
                std::vector<unsigned char>t_vec) noexcept(false);

        //! \brief A trivial destructor
        ~Table() = default;
    };



    /*!
     * \brief This class describes a program.
     *
     * The Program class is used to store the Entities descriptions.
     * It allows the user to request the table to allocate memory
     * for Entities, get information about them,
     * work with the existing Entities that this Program has access to,
     * and refuse the Entities by deallocating the memory.
     */
    class Program{
    private:
        std::vector<Entity*> entities;   ///< the entities the program can operate with
        std::string file_address;        ///< file address string
        const size_t memory_quota;       ///< max amount of memory available to this program
        Table* table;                    ///< a program has no meaning w/o a table to store data in
        static const int menus;          ///< menus amount
        static std::string menu[];       ///< menus
        std::mutex mtx;                     ///< The mutex object protecting from multitasking errors
        std::condition_variable not_empty;   ///< A condition variable signalizing the program can be written to
        std::condition_variable not_full;  ///< A condition variable signalizing the program can be read from

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

        //! \brief A copying '=' operator
        Program& operator =(const Program&);

        //! \brief A moving '=' operator
        Program& operator =(Program&&) noexcept;

        //! \brief A '==' operator used to compare Program's equality
        bool operator ==(const Program&);

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

        //! \brief a method to get an Entity at the given index
        const Entity* get_entity(size_t index) const noexcept(false);

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
         * \brief A method to get all Dividable Segments available.
         * \return A vector of the added Dividable Segments
         * \sa Entity, DivSeg
         */
        std::vector<Entity*> get_div_segs() noexcept;

        /*!
         * \brief A method to add existing Dividable Segments to this Program.
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



    /*!
     * \brief This class is used to contain and give access to Programs.
     *
     * The App class is used to store the Program class objects
     * and provides dialogue methods to work with them.
     */
    class App{
    private:
        Table* table;                    ///< A pointer to a table objects existing in this App
        std::vector<Program*> programs;  ///< A vector of existing Programs in this App
    public:

        //! \brief A default constructor, creates a new Table.
        App();

        //! \brief A dialogue running the App method.
        void run();

        //! \brief A dialogue method for creating a Program.
        void create_program();

        //! \brief A method to command an existing Program
        int command();

        //! \brief A method to add an existing Dividable Segment to a certain Program
        void add_ds();

        //! \brief A dialogue method to list all Programs
        void list_programs();

        //! An obvious destructor deleting Programs and the Table pointers
        ~App();
    };



    /*!
     * \brief This class describes a single Value.
     *
     * The Value class is a class derived from the Entity class.
     * It contains the methods for setting the value,
     * reading it from the table and converting it into
     * human-readable numbers.
     */
    class Value : public Entity{
    public:

        //! \brief A default constructor of a Value. Usually not used directly.
        Value() = default;

        //! \brief A copying constructor
        Value(const Value&) = default;

        //! \brief A moving constructor
        Value(Value&&) noexcept;

        /*!
         * \brief A method which shows all the information about this Value.
         * \param table the table which this Value is stored in
         * \param os the output stream to print the information to
         * \sa Entity
         */
        std::ostream& show(const Table& table, std::ostream& os) const override;

        /*!
         * \brief A method which creates a clone of this Value.
         * \return A newly created object
         * \sa Entity
         */
        Entity* clone() const override;

        /*!
         * \brief A method which creates a Link to this Value.
         * \param t_name the name of the new Link
         * \return A newly created Link
         * \sa Entity
         */
        Entity* create_link(std::string t_name) const override;

        /*!
         * \brief A dialogue method which runs the Value dialogue.
         * \sa Entity
         */
        std::ostream& run(Table&, std::ostream&) override;


        /*!
         * \brief A method which return the instance stored in the table described by this Value.
         * \param table the table this Value stores the information in
         * \return The value of this object
         */
        unsigned long long get_instance(const Table& table) const;

        /*!
         * \brief A method to set the instance of this Value.
         * \param table the table this Value is stored in
         * \param new_inst the new instance to be set
         */
        void set_instance(Table& table, unsigned long long new_inst) noexcept(false);

        //! \brief A simple trivial destructor.
        ~Value() override = default;
    };



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
    class Link : public Entity{
    private:
        Entity* ptr;      ///< the pointer to the Entity this Link points to
    public:

        //! \brief A default Link costructor. Usually not used directly.
        Link() = delete;

        //! \brief A constructor used to create a Link with the given name and existing Entity pointer.
        explicit Link(Entity*, std::string t_name);

        //! \brief A copying constructor for a Link.
        Link(const Link&);

        //! \brief A moving constructor of a Link.
        Link(Link&&) noexcept;


        /*!
         * \brief A method which shows all the information about this Link.
         * \param table the table which this Link is stored in
         * \param os the output stream to print the information to
         * \sa Entity
         */
        std::ostream& show(const Table& table, std::ostream& os) const override;

        /*!
         * \brief A method which creates a clone of this Link.
         * \return A newly created object
         * \sa Entity
         */
        Entity* clone() const override;

        /*!
        * \brief A method which creates a Link to this Link.
        * \param t_name the name of the new Link
        * \return A newly created Link
        * \sa Entity
        */
        Entity* create_link(std::string t_name) const override;

        /*!
         * \brief A dialogue method which runs the Link dialogue.
         * \sa Entity
         */
        std::ostream& run(Table&, std::ostream&) override;


        /*!
         * \brief A method which return the instance stored in the Entity pointed by this Link.
         * \param table the table the Entity pointed on by this Link stores information in
         * \return The value of the Entity pointed on by this Link
         */
        unsigned long long get_instance(const Table& table) const;

        /*!
         * \brief A method to set the instance of the Entity pointed on by this Link.
         * \param table the table the instance of the Entity pointed on by this Link is stored in
         * \param new_inst the new instance to be set
         * \param index the index of the element to be set in case the Link points to and Array-type
         */
        void set_instance(Table& table, unsigned long long new_inst, size_t index = 0);

        /*!
         * \brief A method to get a pointer to the core Entity this Link points to.
         * \return The pointer to the core Entity this Link points to
         * \sa Entity, create_link(std::string t_name)
         */
        Entity* get_core_entity() const;

        //! \brief a trivial destructor.
        ~Link() override = default;
    };



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
    class Array : public Entity{
    public:

        //! \brief The default constructor of an Array. Usually not used directly.
        Array() = default;

        //! \brief A copying constructor of an Array.
        Array(const Array&) = default;

        //! \brief A moving constructor of an Array.
        Array(Array&&) noexcept;

        /*!
         * \brief A method which shows all the information about this Array.
         * \param table the table which this Array is stored in
         * \param os the output stream to print the information to
         * \sa Entity
         */
        std::ostream& show(const Table& table, std::ostream& os) const override;

        /*!
        * \brief A method which creates a clone of this Array.
        * \return A newly created object
        * \sa Entity
        */
        Entity* clone() const override;

        /*!
         * \brief A method which creates a Link to this Array.
         * \param t_name the name of the new Array
         * \return A newly created Link
         * \sa Entity
         */
        Entity* create_link(std::string t_name) const override;

        /*!
         * \brief A dialogue method which runs the Array dialogue.
         * \sa Entity
         */
        std::ostream& run(Table&, std::ostream&) override;

        /*!
         * \brief A method which returns a single Array instance.
         * \param table the table this Array stores the data in
         * \param t_index the index of the needed element of the Array
         * \return  the instance of a certain element
         * \sa Entity
         */
        unsigned long long get_single_instance(const Table& table, size_t t_index) const noexcept(false);

        /*!
        * \brief A method to set the instance of this Array.
        * \param table the table this Array is stored in
         *\param where the location of the element of the Array to be set
        * \param what the new instance to be set
        */
        void set_single_instance(Table& table, size_t where, unsigned long long what) noexcept(false);

        /*!
        * \brief The operator which allowing to get multiple instances of the Array in the given range.
        * \param table the table this Array is stored in
        * \param t_begin the starter address of the range
        * \param t_end the the end address of the range
        * \sa Entity
        */
        std::vector<unsigned long long> operator ()(const Table& table,
                size_t t_begin,
                size_t t_end) noexcept(false);

        //! \brief A trivial destructor of the Array descriptor.
        ~Array() override = default;
    };



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
    class DivSeg : public Array{
    protected:
        std::vector<Program*> programs;    ///< The programs which have access to this Dividable Segment
        std::mutex mtx;                     ///< The mutex object protecting from multitasking errors
    public:

        //! \brief The default trivial constructor of a Dividable Segment. Usually not used directly.
        DivSeg() = default;

        //! \brief A copying constructor of a Dividable Segment.
        DivSeg(const DivSeg&);

        //! \brief a moving constructor of a Dividable Segment.
        DivSeg(DivSeg&&) noexcept;

        /*!
        * \brief A method which returns a single Dividable Segment instance.
        * \param table the table this Dividable Segment stores the data in
        * \param t_index the index of the needed element of the Array
        * \return  the instance of a certain element
        * \sa Entity
        */
        unsigned long long get_single_instance(const Table& table, size_t t_index) noexcept(false);

        /*!
        * \brief A method to set the instance of this Dividable Segment.
        * \param table the table this Dividable Segment is stored in
         *\param where the location of the element of the Array to be set
        * \param what the new instance to be set
        */
        void set_single_instance(Table& table, size_t where, unsigned long long what) noexcept(false);

        /*!
         * \brief A method which shows all the information about this Dividable Segment.
         * \param table the table which this Dividable Segment is stored in
         * \param os the output stream to print the information to
         * \sa Entity
         */
        std::ostream& show(const Table&, std::ostream&) const override;

        /*!
        * \brief A method which creates a clone of this Dividable Segment.
        * \return A newly created object
        * \sa Entity
        */
        Entity* clone() const override;

        /*!
         * \brief A method which creates a Link to this Dividable Segment.
         * \param t_name the name of the new Dividable Segment
         * \return A newly created Link
         * \sa Entity
         */
        Entity* create_link(std::string) const override;

        /*!
        * \brief A dialogue method which runs the Dividable Segment dialogue.
        * \sa Entity
        */
        std::ostream& run(Table&, std::ostream&) override;


        /*!
        * \brief A method which briefly shows the information about the Programs this DivSeg is stored in.
        * \sa Program
        */
        std::ostream& show_programs (std::ostream&) const;

        /*!
        * \brief A dialogue method which adds a certain program to this Segment's program list.
        * \sa Program
        */
        void add_program(Program* pr);

        /*!
        * \brief A dialogue method which erases a certain program from this Segment's program list.
        * \sa Program
        */
        void erase_program(Program* pr);

        //! \brief The destructor of the Dividable Segment.
        ~DivSeg() override;
    };

}

#endif //MEMORY_MANAGER_MANAGER_H
