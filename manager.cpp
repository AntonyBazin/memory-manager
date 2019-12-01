//
// Created by antony on 10/29/19.
//

#include "manager.h"

namespace manager{

    template<class T>
    Entity *Entity::create_Entity(Entity_ID id) noexcept(false) {
        Entity* ptr;
        switch(id){
            case Value_ID:
                ptr = new Value<T>;
                break;

            case Array_ID:
                ptr = new Array<T>;
                break;

            case DivSeg_ID:
                ptr = new DivSeg<T>;
                break;

            default:
                throw std::domain_error("unknown entity id");
        }
        return ptr;
    }



    void Entity::set_pos(Unit un) {
        position = un;
    }



    size_t Entity::memory_used() const {
        return position.size;
    }



    Unit Entity::get_pos() const{
        return position;
    }


    template<class T>
    T Value<T>::get_instance(Table& table) {
        unsigned int v = 0;
        auto rc = table.read_bytes(position.starter_address, position.size);
        for(size_t i = 0; i < position.size; ++i){
            v = v | (rc[i] << ((position.size - 1)*8 - i*8));
        }
        return v;
    }



    template<class T>
    void Value<T>::set_instance(Table& table, T new_inst) noexcept(false) {
        size_t size = sizeof(T);
        unsigned char c[size];
        auto p = reinterpret_cast<unsigned char *>(&new_inst);
        for(size_t i = 0; i < size; ++i){
            c[size - 1 - i] = p[i];
        }
        std::vector<unsigned char> v(c, c + size);
        table.write(position.starter_address, position.size, v);
    }



    template<class T>
    size_t Value<T>::get_size() {
        return sizeof(T);
    }



    template<class T>
    Entity* Value<T>::create_link() const {
        Entity *ptr = new Link<T>(*this);
        return ptr;
    }



    template<class T>
    Link<T>::Link(const Entity* val) {
        Entity* ent;
        switch(ptr->get_id()){
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

                switch(ptr->get_id()) {
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



    template<class T>
    T Link<T>::get_instance(Table& table) { //todo
        if(this->id == Value_ID){
            return dynamic_cast<Value<T>*>(ptr)->get_instance(table);
        } else {
            return dynamic_cast<DivSeg<T>*>(ptr)->get_instance(table);
        }
    }



    template<class T> // todo
    void Link<T>::set_instance(Table& table, T new_inst) noexcept(false) {
        if(this->id == Value_ID){
            dynamic_cast<Value<T>*>(ptr)->set_instance(table, new_inst);
        } else{
            dynamic_cast<DivSeg<T>*>(ptr)->set_instance(table, new_inst);
        }
    }



    template<class T>
    Entity *Link<T>::get_core_entity() {
        if(ptr->get_id() != Link_ID){
            return ptr;
        } else{
            Entity* ent = ptr;
            while(ent->get_id() == Link_ID){  // getting out of all links
                ent = dynamic_cast<Link<T>*>(ent)->ptr;
            }
            return ent;
        }
    }


    template<class T>
    std::vector<T> Array<T>::operator()(Table &, size_t t_begin, size_t t_end) {
        return std::vector<T>();
    }

}