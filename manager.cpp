//
// Created by antony on 10/29/19.
//

#include "manager.h"

namespace manager{


    Entity *Entity::generate_Entity(Entity_ID e_id, Type_ID type, const std::string& t_name) noexcept(false) {
        Entity* ptr;
        switch(type){
            case CHAR:
                ptr = Entity::create_Entity<char>(e_id, t_name);
                ptr->t_id = type;
                break;
            case INT:
                ptr = Entity::create_Entity<int>(e_id, t_name);
                ptr->t_id = type;
                break;
            case LONG:
                ptr = Entity::create_Entity<long>(e_id, t_name);
                ptr->t_id = type;
                break;
            case LONGLONG:
                ptr = Entity::create_Entity<long long>(e_id, t_name);
                ptr->t_id = type;
                break;
            case FLOAT:
                ptr = Entity::create_Entity<float>(e_id, t_name);
                ptr->t_id = type;
                break;
            case DOUBLE:
                ptr = Entity::create_Entity<double>(e_id, t_name);
                ptr->t_id = type;
                break;
            case LONGDOUBLE:
                ptr = Entity::create_Entity<long double>(e_id, t_name);
                ptr->t_id = type;
                break;
            default:
                throw std::domain_error("Unexpected type id");
        }
        return ptr;
    }



    template<class T>
    Entity *Entity::create_Entity(Entity_ID id, std::string t_name) noexcept(false) {
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
        ptr->set_name(std::move(t_name));
        return ptr;
    }



    size_t Entity::memory_used() const noexcept{
        return position.size;
    }



    Unit Entity::get_pos() const noexcept{
        return position;
    }

    Entity *Entity::create_link() const {


        return nullptr;
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
    Link<T>::Link(const Entity* val) {
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



    template<class T>
    T Link<T>::get_instance(Table& table) {
        if(this->e_id == Value_ID){
            return dynamic_cast<Value<T>*>(ptr)->get_instance(table);
        } else {
            return dynamic_cast<DivSeg<T>*>(ptr)->get_instance(table);
        }
    }



    template<class T> // todo
    void Link<T>::set_instance(Table& table, T new_inst) noexcept(false) {
        if(this->e_id == Value_ID){
            dynamic_cast<Value<T>*>(ptr)->set_instance(table, new_inst);
        } else{
            dynamic_cast<DivSeg<T>*>(ptr)->set_instance(table, new_inst);
        }
    }



    template<class T>
    Entity *Link<T>::get_core_entity() {
        if(ptr->get_entity_id() != Link_ID){
            return ptr;
        } else{
            Entity* ent = ptr;
            while(ent->get_entity_id() == Link_ID){  // getting out of all links
                ent = dynamic_cast<Link<T>*>(ent)->ptr;
            }
            return ent;
        }
    }



    template<class T>
    std::vector<T> Array<T>::operator()(Table&, size_t t_begin, size_t t_end) { //todo

        return std::vector<T>();
    }


}