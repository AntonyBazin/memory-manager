//
// Created by antony on 11/19/19.
//

#include "manager.h"


namespace manager{


    Table::Table() {
        current_size = 0;
        memory.insert(memory.begin(), max_size, '\0');
        free_blocks = {};
        free_blocks.emplace_back(0, max_size, false);
    }



    void Table::defragmentation() {
        std::vector<Unit>::iterator vec_it;  // a cycle is used for full defragmentation
        for(vec_it = free_blocks.begin() + 1; vec_it != free_blocks.end(); ++vec_it){
            if(vec_it->starter_address == (vec_it - 1)->starter_address + (vec_it - 1)->size){
                --vec_it;
                unsigned int sz = (vec_it + 1)->size;
                free_blocks.erase(vec_it + 1);
                vec_it->size += sz;
            }
        }
    }



    void Table::mark_free(unsigned int t_strt, unsigned int t_size) noexcept(false) {
        if(t_strt < 0)
            throw std::out_of_range("starter address below zero");
        if(t_strt > max_size)
            throw std::out_of_range("starter address higher than max size");
        auto vec_it = free_blocks.begin();
        for(; vec_it != free_blocks.end(); ++vec_it){  // checks for invalid
            unsigned int current_start = vec_it->starter_address;
            unsigned int current_end = vec_it->starter_address + vec_it->size;

            if(current_start <= t_strt && current_end >= t_strt)
                throw std::invalid_argument("attempt to free memory with start before or at arg");
            if(current_start > t_strt && current_end < t_strt + t_size)
                throw std::invalid_argument("attempt to free memory with end after or at arg");
        }

        auto mark = std::find_if(free_blocks.begin(),
                                 free_blocks.end(),
                                 [t_strt](Unit un) -> bool { return un.starter_address > t_strt; });
        Unit newcomer(t_strt, t_size, false);
        free_blocks.insert(mark, newcomer);
    }



    Entity* Table::allocate_memory(unsigned int t_size, Entity_ID id) noexcept(false) {
        auto mark = std::find_if(free_blocks.begin(),
                                 free_blocks.end(),
                                 [t_size](Unit un) -> bool { return un.size >= t_size; });

        if(mark == free_blocks.end()){
            defragmentation();
            mark = std::find_if(free_blocks.begin(),
                                free_blocks.end(),
                                [t_size](Unit un) -> bool { return un.size > t_size; });
            if(mark == free_blocks.end()) throw std::runtime_error("not enough memory");
        }

        unsigned int strt = mark->starter_address;

        if(mark->size == t_size){
            free_blocks.erase(mark);
        } else{
            mark->starter_address += t_size;
            mark->size -= t_size;
        }

        Entity* new_entity = Entity::create_Entity(id);
        Unit pos(strt, t_size, true);
        new_entity->set_pos(pos);
        return new_entity;
    }


}