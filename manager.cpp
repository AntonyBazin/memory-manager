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

    size_t Entity::memory_used() {
        return position.size;
    }

}