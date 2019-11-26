//
// Created by antony on 10/29/19.
//

#include "manager.h"

namespace manager{


    Entity *Entity::create_Entity(Entity_ID id) noexcept(false) {
        Entity* ptr;
        switch(id){
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


    unsigned int Value::get_instance(Table& table) {
        unsigned int v = 0;
        auto rc = table.read_bytes(position.starter_address, position.size);
        for(size_t i = 0; i < position.size; ++i){
            v = v | (rc[i] << ((position.size - 1)*8 - i*8));
        }
        return v;
    }

    void Value::set_instance(Table& table, unsigned int t_new_inst) noexcept(false) {
        unsigned char c[sizeof(int)];
        auto p = reinterpret_cast<unsigned char *>(&t_new_inst);
        for(int i = 0; i < 4; ++i){
            c[3 -i] = p[i];
        }
        std::vector<unsigned char> v(c, c + sizeof(int));
        table.write(position.starter_address, position.size, v);
    }
}