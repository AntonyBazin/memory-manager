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



    long long Value::get_instance(const Table& table) const {
        long long v = 0;
        auto rc = table.read_bytes(position.starter_address, position.size);
        for(size_t i = 0; i < get_size(); ++i){
            v = v | (rc[i] << ((get_size() - 1)*8 - i*8));
        }
        return v;
    }



    void Value::set_instance(Table& table, long long new_inst) noexcept(false) {
        size_t size = get_size();
        unsigned char c[size];
        auto p = reinterpret_cast<unsigned char *>(&new_inst);
        for(size_t i = 0; i < size; ++i){
            c[size - 1 - i] = p[i];
        }
        std::vector<unsigned char> v(c, c + size);
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
        os << get_instance(table);
        return os;
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
        this->e_id = ent->get_entity_id();
        this->refs = 0;
        this->ptr = ent;
        this->name = std::move(t_name);
    }



    long long Link::get_instance(const Table& table) const{
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



    void Link::set_instance(Table& table, long long new_inst, size_t index) noexcept(false) {
        auto cr = get_core_entity();
        switch(cr->get_entity_id()) {
            case Value_ID:
                (dynamic_cast<Value*>(cr))->set_instance(table, new_inst);
            case Array_ID:
                (dynamic_cast<Array*>(cr))->set_single_instance(table, index, new_inst);
            case DivSeg_ID:
                (dynamic_cast<DivSeg*>(cr))->set_single_instance(table, index, new_inst);
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
        os << get_instance(table);
        return os;
    }



    Array::Array(Array&& arr) noexcept {
        this->e_id = arr.e_id;
        this->name = arr.name;
        this->position = arr.position;
        this->refs = arr.refs;
    }



    void Array::set_single_instance(Table& table, size_t where, long long what) {
        size_t size = single_size;
        unsigned char c[size];
        auto p = reinterpret_cast<unsigned char *>(&what);
        for(size_t i = 0; i < size; ++i){
            c[size - 1 - i] = p[i];
        }
        std::vector<unsigned char> v(c, c + size);
        table.write(position.starter_address + (size*where), position.size, v);
    }




    long long Array::get_single_instance(const Table& table, size_t t_index) const{
        size_t size = single_size;
        long long v = 0;
        auto rc = table.read_bytes(position.starter_address + (t_index*size), size);

        for(size_t i = 0; i < size; ++i){
            v = v | (rc[i] << ((size - 1)*8 - i*8));
        }
        return v;
    }



    std::vector<long long> Array::operator()(const Table& table, size_t t_begin, size_t t_end) {
        std::vector<long long> vec;
        for(size_t i = t_begin; i < t_end; ++i){
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
        size_t start = this->position.starter_address;
        size_t end = this->position.starter_address + this->position.size;
        for(size_t i = start; i < end; ++i){
            os << get_single_instance(table, i) << " ";
        }
        return os;
    }



    void DivSeg::add_program(Program &pr) noexcept(false) {
        if(this->e_id != DivSeg_ID)
            throw std::domain_error("Cannot add a program to a non-DivSeg element");
        programs.push_back(pr);
    }



    void DivSeg::erase_program(Program& pr) noexcept(false) {
        if(this->e_id != DivSeg_ID)
            throw std::domain_error("Cannot erase a program of a non-DivSeg element");
        auto pos = std::find(programs.begin(), programs.end(), pr);
        programs.erase(pos);
    }



    DivSeg::DivSeg(const DivSeg& ds) : Array(ds) {
        for(const auto program : ds.programs){
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
        for(auto it = programs.begin(); it != programs.end(); ++it){
            os << (*it).get_address() << " ";
        }
        return os;
    }



    int App::command() {
        int i = 0;
        std::cout << "Which program to run?" << std::endl;
        try{
            for(auto pr = programs.begin(); pr != programs.end(); ++pr, ++i){
                std::cout << i << pr->get_address() << std::endl;
            }
        } catch(std::exception& ex){
            std::cout << ex.what() << std::endl;
            return 1;
        }
        std::cin >> i;
        try{
            programs.at(i).run();
        } catch(std::exception& ex){
            std::cout << ex.what() << std::endl;
            return 1;
        }
        return 1;
    }



    void App::create_program() {
        size_t q;
        std::string name;
        std::cout << "Enter program's name: ";
        std::cin >> name;
        std::cout << std::endl << "Enter the program's memory quota: ";
        std::cin >> q;
        Program pr(table, 200, name);
    }



    void App::run() {
        int rc = 1;
        while(rc != 0){
            std::cout << "What to do?";
            std::cout << "0 - quit;" << std::endl
                      << "1 - add program;" << std::endl
                      << "2 - run program." << std::endl;
            std::cin >> rc;
            switch(rc){
                case 0:
                    rc = 0;
                case 1:
                    create_program();
                    break;
                case 2:
                    command();
                    break;
                default:
                    std::cout << "unexpected input. Try again." << std::endl;
                    rc = 0;
            }

        }
    }
}