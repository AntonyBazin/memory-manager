//
// Created by antony on 10/29/19.
//

#include "manager.h"


namespace manager{


    Entity* Entity::generate_Entity(Entity_ID e_id,
            size_t single_size,
            const std::string& t_name) noexcept(false) {

        Entity* ptr;
        switch(e_id){
            case Value_ID:
                ptr = new Value;
                break;

            case Array_ID:
                ptr = new Array;
                break;

            case DivSeg_ID:
                ptr = new DivSeg;
                break;

            default:
                throw std::domain_error("unknown entity id");
        }
        ptr->set_id(e_id);
        ptr->set_name(t_name);
        ptr->set_single_size(single_size);
        return ptr;
    }



    Entity::Entity(const Entity& ent) {
        this->e_id = ent.e_id;
        this->name = ent.name;
        this->position = ent.position;
        this->refs = ent.refs;
        this->single_size = ent.single_size;
    }



    Entity::Entity(Entity&& ent) noexcept {
        this->e_id = ent.e_id;
        this->name = ent.name;
        this->position = ent.position;
        this->refs = ent.refs;
        this->single_size = ent.single_size;
    }



    Value::Value(Value&& val) noexcept {
        this->e_id = val.e_id;
        this->name = val.name;
        this->position = val.position;
        this->refs = val.refs;
    }



    unsigned long long Value::get_instance(const Table& table) const {
        unsigned long long v = 0;
        auto rc = table.read_bytes(position.starter_address, position.size);
        for(size_t i = 0; i < get_size(); ++i){
            v = v | (rc[i] << ((get_size() - 1)*8 - i*8));
        }
        return v;
    }



    void Value::set_instance(Table& table, unsigned long long new_inst) noexcept(false) {
        size_t size = get_size();
        unsigned char c[size];
        auto p = reinterpret_cast<unsigned char *>(&new_inst);
        for(size_t i = 0; i < size; ++i){
            c[size - 1 - i] = p[i];
        }
        std::vector<unsigned char>::iterator i1(c);
        std::vector<unsigned char>::iterator i2 = i1 + size;
        std::vector<unsigned char> v(i1, i2);
        table.write(position.starter_address, position.size, v);
    }



    Entity* Value::clone() const {
        auto val = new Value(*this);
        return val;
    }



    Entity* Value::create_link(std::string t_name) const {
        Link* lnk = new Link(const_cast<Value*>(this), t_name);
        return lnk;
    }



    std::ostream& Value::show(const Table& table, std::ostream& os) const {
        os << get_name() << ":" << std::endl
           << get_instance(table) << std::endl;
        return os;
    }



    std::ostream& Value::run(Table& table, std::ostream& os) {
        size_t ct;
        unsigned long long val;
        while(true){
            os << std::endl << "Value " << this->get_name() << std::endl;
            os << std::endl << "Choose action:" << std::endl
               << "0 - go back;" << std::endl
               << "1 - print value;" << std::endl
               << "2 - set value." << std::endl;
            std::cin >> ct;
            switch(ct){
                case 0:
                    os << "Going back..." << std::endl;
                    return os;
                case 1:
                    this->show(table, std::cout);
                    os << std::endl;
                    break;
                case 2:
                    std::cout << "Enter value to be set: ";
                    std::cin >> val;
                    this->set_instance(table, val);
                    break;
                default:
                    std::cout << "Unexpected choice, try again!" << std::endl;
                    break;
            }
        }
    }



    Link::Link(const Link& lnk)  : Entity(lnk) {
        ptr = lnk.ptr;
    }



    Link::Link(Link&& lnk) noexcept {
        this->e_id = lnk.e_id;
        this->name = lnk.name;
        this->position = lnk.position;
        this->refs = lnk.refs;
        ptr = lnk.ptr;
        lnk.ptr = nullptr;
    }



    Entity* Link::clone() const {
        Link* lnk = new Link(*this);
        return lnk;
    }



    Link::Link(Entity* ent , std::string t_name) {
        this->position = ent->get_pos();
        this->e_id = Link_ID;
        this->refs = 0;
        this->ptr = ent;
        this->name = std::move(t_name);
    }



    unsigned long long Link::get_instance(const Table& table) const{
        auto cr = get_core_entity();
        switch(cr->get_entity_id()) {
            case Value_ID:
                return (dynamic_cast<Value*>(cr))->get_instance(table);
            case Array_ID:
                return (dynamic_cast<Array*>(cr))->get_single_instance(table, 0);
            case DivSeg_ID:
                return (dynamic_cast<DivSeg*>(cr))->get_single_instance(table, 0);
            default:
                throw std::domain_error("unexpected core entity on getter");
        }
    }



    void Link::set_instance(Table& table, unsigned long long new_inst, size_t index) noexcept(false) {
        auto cr = get_core_entity();
        switch(cr->get_entity_id()) {
            case Value_ID:
                (dynamic_cast<Value*>(cr))->set_instance(table, new_inst);
                break;
            case Array_ID:
                (dynamic_cast<Array*>(cr))->set_single_instance(table, index, new_inst);
                break;
            case DivSeg_ID:
                (dynamic_cast<DivSeg*>(cr))->set_single_instance(table, index, new_inst);
                break;
            default:
                throw std::domain_error("unexpected core entity on setter");
        }
    }




    Entity* Link::get_core_entity() const {
        if(ptr->get_entity_id() != Link_ID){ //if core entity is something that is not a link
            return ptr;
        } else{
            Entity* ent = ptr;
            while(ent->get_entity_id() == Link_ID){  // getting out of all links
                ent = (dynamic_cast<Link*>(ent))->ptr;
            }
            return ent;
        }
    }



    Entity* Link::create_link(std::string t_name) const {
        Link* lnk = new Link(const_cast<Link*>(this), t_name);
        return lnk;
    }



    std::ostream& Link::show(const Table& table, std::ostream& os) const {
        os << get_name() << ":" << std::endl
           << get_instance(table) << std::endl;
        return os;
    }



    std::ostream& Link::run(Table& table, std::ostream& os) {
        size_t ct;
        unsigned long long val;

        while(true){
            os << "Link " << this->get_name();
            os << " to " << this->get_core_entity()->get_name() << std::endl << std::endl;
            os << "Choose action:" << std::endl
               << "0 - go back;" << std::endl
               << "1 - print value" << std::endl
               << "2 - set value" << std::endl;
            std::cin >> ct;
            switch(ct){
                case 0:
                    os << "Going back..." << std::endl;
                    return os;
                case 1:
                    this->show(table, os);
                    os << std::endl;
                    break;
                case 2:
                    os << "Enter value to be set: ";
                    std::cin >> val;
                    this->set_instance(table, val);
                    os << std::endl;
                    break;
                default:
                    os << "Unexpected choice, try again!" << std::endl;
                    break;
            }
        }
    }



    Array::Array(Array&& arr) noexcept {
        this->e_id = arr.e_id;
        this->name = arr.name;
        this->position = arr.position;
        this->refs = arr.refs;
    }



    void Array::set_single_instance(Table& table, size_t where, unsigned long long what) noexcept(false) {
        if(where >= position.size/single_size)
            throw std::runtime_error("There is no such element in the array!");
        if(what > static_cast<unsigned long long>(std::pow(2, single_size*8)))
            throw std::runtime_error("The argument is too high to contain!");

        size_t size = single_size;
        unsigned char c[size];
        auto p = reinterpret_cast<unsigned char *>(&what);
        for(size_t i = 0; i < size; ++i){
            c[size - 1 - i] = p[i];
        }
        std::vector<unsigned char>::iterator i1(c);
        std::vector<unsigned char>::iterator i2 = i1 + size;
        std::vector<unsigned char> v(i1, i2);
        table.write(position.starter_address + (size*where), single_size, v);
    }




    unsigned long long Array::get_single_instance(const Table& table, size_t t_index) const noexcept(false) {
        if(t_index > this->position.size / single_size)
            throw std::runtime_error("Unexpected index to read!");

        size_t size = single_size;
        unsigned long long v = 0;
        auto rc = table.read_bytes(position.starter_address + (t_index*size), size);

        for(size_t i = 0; i < size; ++i){
            v = v | (rc[i] << ((size - 1)*8 - i*8));
        }
        return v;
    }



    std::vector<unsigned long long> Array::operator()(const Table& table,
            size_t t_begin,
            size_t t_end) noexcept(false) {

        if(t_begin > (this->position.size)/single_size)
            throw std::invalid_argument("Incorrect first index");
        if(t_end > (this->position.size)/single_size)
            throw std::invalid_argument("Incorrect second index");
        std::vector<unsigned long long> vec;
        for(size_t i = t_begin; i <= t_end; ++i){
            vec.push_back(get_single_instance(table, i));
        }
        return vec;
    }



    Entity* Array::clone() const {
        auto arr = new Array(*this);
        return arr;
    }



    Entity* Array::create_link(std::string t_name) const {
        Link* lnk = new Link(const_cast<Array*>(this), t_name);
        return lnk;
    }



    std::ostream& Array::show(const Table& table, std::ostream& os) const {
        os << get_name() << ":" << std::endl;
        for(size_t i = 0; i < ((this->position.size)/single_size); ++i){
            os << get_single_instance(table, i) << " ";
        }
        os << std::endl;
        return os;
    }



    std::ostream& Array::run(Table& table, std::ostream& os) {
        size_t ct;
        size_t i1, i2; // indexes
        unsigned long long val;

        while(true){
            os << "Array " << this->get_name();
            os << "[" << (this->position.size / single_size) << "]" << std::endl << std::endl;
            os <<  "Choose action:" << std::endl
               << "0 - go back;" << std::endl
               << "1 - print values" << std::endl
               << "2 - set value by index" << std::endl
               << "3 - print values by indexes" << std::endl;
            std::cin >> ct;
            switch(ct){
                case 0:
                    os << "Going back..." << std::endl;
                    return os;
                case 1:
                    this->show(table, os);
                    os << std::endl;
                    break;
                case 2:
                    os << "Enter index: ";
                    std::cin >> i1;
                    os << "Enter value: ";
                    std::cin >> val;
                    try{
                        this->set_single_instance(table, i1, val);
                    } catch(std::exception& ex){
                        std::cerr << "Array::run: " << ex.what() << std::endl;
                    }

                    break;
                case 3:
                    os << "Enter the start and end indexes: ";
                    std::cin >> i1 >> i2;
                    try{
                        auto vec = (*this)(table, i1, i2);
                        os << "The array between " << i1 << " and " << i2 << std::endl;
                        for(unsigned long long i : vec){
                            std::cout << " " << i;
                        }
                    } catch(std::exception& ex){
                        std::cerr << ex.what() << std::endl;
                        return os;
                    }
                    break;
                default:
                    os << "Unexpected choice, try again!" << std::endl;
                    break;
            }
        }
    }



    void DivSeg::add_program(Program* pr) noexcept(false) {
        if(this->e_id != DivSeg_ID)
            throw std::domain_error("Cannot add a program to a non-DivSeg element");
        if(std::find(programs.begin(), programs.end(), pr) != programs.end())
            throw std::domain_error("Program already added");

        programs.push_back(pr);
    }



    void DivSeg::erase_program(Program* pr) noexcept(false) {
        if(this->e_id != DivSeg_ID)
            throw std::domain_error("Cannot erase a program of a non-DivSeg element");
        auto pos = std::find(programs.begin(), programs.end(), pr);
        programs.erase(pos);
    }



    DivSeg::DivSeg(const DivSeg& ds) : Array(ds) {
        for(const auto& program : ds.programs){
            this->programs.push_back(program);
        }
    }



    DivSeg::DivSeg(DivSeg&& ds) noexcept {
        this->e_id = ds.e_id;
        this->name = ds.name;
        this->position = ds.position;
        this->refs = ds.refs;
        std::move(ds.programs.begin(),
                  ds.programs.end(),
                  this->programs.begin());
        ds.programs.clear();
    }



    Entity* DivSeg::clone() const {
        auto ds = new DivSeg(*this);
        return ds;
    }



    Entity* DivSeg::create_link(std::string t_name) const {
        Link* lnk = new Link(const_cast<DivSeg*>(this), t_name);
        return lnk;
    }



    std::ostream& DivSeg::show(const Table& table, std::ostream& os) const {
        return Array::show( table , os);
    }



    std::ostream& DivSeg::show_programs(std::ostream& os) const {
        for(auto program : programs){
            os << program->get_address() << std::endl;
        }
        return os;
    }



    std::ostream& DivSeg::run(Table& table, std::ostream& os) {
        size_t ct;
        size_t i1, i2;
        unsigned long long val;

        while(true){
            os << "DivSeg " << this->get_name();
            os << "[" << (this->position.size / single_size) << "]" << std::endl << std::endl;
            os << "Choose action:" << std::endl
               << "0 - go back;" << std::endl
               << "1 - print values" << std::endl
               << "2 - set value by index" << std::endl
               << "3 - print values by indexes" << std::endl
               << "4 - print programs" << std::endl;
            std::cin >> ct;
            switch(ct){
                case 0:
                    os << "Going back..." << std::endl;
                    return os;
                case 1:
                    this->show(table, os);
                    os << std::endl;
                    break;
                case 2:
                    os << "Enter index: ";
                    std::cin >> i1;
                    os << "Enter value: ";
                    std::cin >> val;
                    try{
                        this->set_single_instance(table, i1, val);
                    } catch(std::exception& ex){
                        std::cerr << "In DivSeg::run: " << ex.what() << std::endl;
                    }
                    break;
                case 3:
                    os << "Enter the start and end indexes: ";
                    std::cin >> i1 >> i2;
                    try{
                        auto vec = (*this)(table, i1, i2);
                        for(unsigned long long i : vec){
                            os << " " << i;
                        }
                    } catch(std::exception& ex){
                        std::cerr << ex.what() << std::endl;
                        return os;
                    }
                    break;
                case 4:
                    os << "Programs:" << std::endl;
                    this->show_programs(os);
                    break;
                default:
                    os << "Unexpected choice, try again!" << std::endl;
                    break;
            }
        }
    }

    DivSeg::~DivSeg() {
        programs.clear();  // intended, this should NEVER destroy the programs it refers to
    }


    bool Unit::operator==(const Unit& un) const {
        return(this->size == un.size && this->starter_address == un.starter_address);
    }
}