//
// Created by antony on 10/29/19.
//

#include "manager.h"

namespace manager{


    Entity *Entity::generate_Entity(Entity_ID e_id, const std::string& t_name) noexcept(false) {
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
        ptr->set_name(t_name);
        return ptr;
    }



    Entity::Entity(const Entity& ent) {
        this->e_id = ent.e_id;
        this->name = ent.name;
        this->position = ent.position;
        this->refs = ent.refs;
    }



    Entity::Entity(Entity&& ent) noexcept {
        this->e_id = ent.e_id;
        this->name = ent.name;
        this->position = ent.position;
        this->refs = ent.refs;
    }



    Value::Value(Value&& val) noexcept {
        this->e_id = val.e_id;
        this->name = val.name;
        this->position = val.position;
        this->refs = val.refs;
    }



    long long Value::get_instance(Table& table) {
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



    Value* Value::clone() const {
        auto val = new Value(*this);
        return val;
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



    Link* Link::clone() const {
        Link* lnk = new Link(*this);
        return lnk;
    }


    /*Link::Link(const Entity* val) {
        Entity* ent;
        switch(ptr->get_entity_id()){
            case Value_ID:
                ent = dynamic_cast<Value<T>*>(val);
                break;
            case Array_ID:
                ent = dynamic_cast<Array<T>*>(val);
                break;
            case DivSeg_ID:
                ent = dynamic_cast<DivSeg<T>*>(val);
                break;
            default:
                ent = dynamic_cast<Link<T>*>(val)->get_core_entity();

                switch(ptr->get_entity_id()) {
                    case Value_ID:
                        ent = dynamic_cast<Value<T> *>(val);
                        break;
                    case Array_ID:
                        ent = dynamic_cast<Array<T> *>(val);
                        break;
                    case DivSeg_ID:
                        ent = dynamic_cast<DivSeg<T> *>(val);
                        break;
                }
                break;
        }
        ptr = ent;
    }



    long long Link::get_instance(Table& table) {
        auto cr = dynamic_cast<Link*>(ptr)->get_core_entity();
        switch(cr->get_entity_id()) {
            case Value_ID:
                return dynamic_cast<Value*>(ptr)->get_instance(table);
            case Array_ID:
                return dynamic_cast<Array*>(ptr)->get_instance(table);
            case DivSeg_ID:
                return dynamic_cast<DivSeg*>(ptr)->get_instance(table);
            default:
                throw std::domain_error("unexpected core entity");
            case Link_ID:
                break;
            case E_ERR:
                break;
        }
    }




    void Link::set_instance(Table& table, T new_inst, size_t index) noexcept(false) {
        auto core = dynamic_cast<Link<T>*>(ptr)->get_core_entity();
        switch(core->get_entity_id()) {
            case Value_ID:
                (dynamic_cast<Value<T>*>(core))->set_instance(table, new_inst);
            case Array_ID:
                (dynamic_cast<Array<T>*>(core))->set_single_instance(index, new_inst);
            case DivSeg_ID:
                (dynamic_cast<DivSeg<T>*>(core))->set_single_instance(index, new_inst);
            default:
                throw std::domain_error("unexpected core entity");
        }
    }




    Entity *Link::get_core_entity() {
        if(ptr->get_entity_id() != Link_ID){ // core entity is something that is not a link
            return ptr;
        } else{
            Entity* ent = ptr;
            while(ent->get_entity_id() == Link_ID){  // getting out of all links
                ent = (dynamic_cast<Link*>(ent))->ptr;
            }
            return ent;
        }
    }*/


    Array::Array(Array&& arr) noexcept {
        this->e_id = arr.e_id;
        this->name = arr.name;
        this->position = arr.position;
        this->refs = arr.refs;
    }



    void Array::set_single_instance(Table& table, size_t where, long long what) {
        size_t size = get_size();
        unsigned char c[size];
        auto p = reinterpret_cast<unsigned char *>(&what);
        for(size_t i = 0; i < size; ++i){
            c[size - 1 - i] = p[i];
        }
        std::vector<unsigned char> v(c, c + size);
        table.write(position.starter_address + (size*where), position.size, v);
    }




    long long Array::get_single_instance(Table& table, size_t t_index) const{
        size_t size = get_size();
        long long v = 0;
        auto rc = table.read_bytes(position.starter_address + (t_index*size), size);

        for(size_t i = 0; i < size; ++i){
            v = v | (rc[i] << ((size - 1)*8 - i*8));
        }
        return v;
    }



    std::vector<long long> Array::operator()(Table& table, size_t t_begin, size_t t_end) {
        std::vector<long long> vec;
        for(size_t i = t_begin; i < t_end; ++i){
            vec.push_back(get_single_instance(table, i));
        }
        return vec;
    }



    Array* Array::clone() const {
        auto arr = new Array(*this);
        return arr;
    }



    void DivSeg::add_program(Program &pr) noexcept(false) {
        if(this->e_id != DivSeg_ID)
            throw std::domain_error("Cannot add a program to a non-DivSeg element");
        programs.push_back(pr);
    }



    void DivSeg::erase_program(Program &pr) noexcept(false) {
        if(this->e_id != DivSeg_ID)
            throw std::domain_error("Cannot erase a program of a non-DivSeg element");
        programs.erase(std::find(programs.begin(), programs.end(), pr));
    }



    DivSeg::DivSeg(const DivSeg& ds) : Array(ds) {
        std::move(ds.programs.begin(),
                  ds.programs.end(),
                  this->programs.begin());
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



    DivSeg* DivSeg::clone() const {
        auto ds = new DivSeg(*this);
        return ds;
    }


}