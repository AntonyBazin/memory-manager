//
// Created by antony on 11/19/19.
//

#include "manager.h"


namespace manager{


    Table::Table() {
        memory.insert(memory.begin(), max_size, '\0');
        free_blocks = {};
        Unit un(0, max_size);
        free_blocks.emplace_back(un);
    }



    void Table::defragmentation() {
        std::vector<Unit>::iterator vec_it;  // a cycle is used for full defragmentation
        for(vec_it = free_blocks.begin() + 1; vec_it != free_blocks.end(); ++vec_it){
            if(vec_it->starter_address == (vec_it - 1)->starter_address + (vec_it - 1)->size + 1){
                --vec_it;
                size_t sz = (vec_it + 1)->size;  //guaranteed not to fail bc of --vec_it
                free_blocks.erase(vec_it + 1);
                vec_it->size += sz;
            }
        }
    }



    void Table::mark_free(size_t t_strt, size_t t_size) noexcept(false) {
        if(t_strt < 0)
            throw std::out_of_range("starter address below zero");
        if(t_strt > max_size)
            throw std::out_of_range("starter address higher than max size");

        auto vec_it = free_blocks.begin();
        for(; vec_it != free_blocks.end(); ++vec_it){  // checks for invalid
            size_t current_start = vec_it->starter_address;
            size_t current_end = vec_it->starter_address + vec_it->size;

            if(current_start <= t_strt && current_end >= t_strt)
                throw std::invalid_argument("attempt to free memory with start before or at arg");
            if(current_start > t_strt && current_end < t_strt + t_size)
                throw std::invalid_argument("attempt to free memory with end after or at arg");
        }

        auto mark = std::find_if(free_blocks.begin(),
                                 free_blocks.end(),
                                 [t_strt](Unit un) -> bool { return un.starter_address > t_strt; });
        Unit newcomer(t_strt, t_size);
        free_blocks.insert(mark, newcomer);
    }



    Unit Table::allocate_memory(size_t t_size) noexcept(false) {
        auto mark = std::find_if(free_blocks.begin(),
                                 free_blocks.end(),
                                 [t_size](Unit un) -> bool { return un.size >= t_size; });

        if(mark == free_blocks.end()){
            defragmentation();
            mark = std::find_if(free_blocks.begin(),
                                free_blocks.end(),
                                [t_size](Unit un) -> bool { return un.size >= t_size; });
            if(mark == free_blocks.end()) throw std::runtime_error("not enough memory");
        }

        size_t strt = mark->starter_address;

        if(mark->size == t_size){
            free_blocks.erase(mark);
        } else{
            mark->starter_address += t_size;
            mark->size -= t_size;
        }

        Unit pos(strt, t_size);
        return pos;
    }



    std::vector<unsigned char> Table::read_bytes(size_t t_strt, size_t t_size) const noexcept(false) {
        if(t_strt < 0 || t_size <= 0)
            throw std::invalid_argument("argument below zero");
        if(t_strt > max_size || t_size > max_size)
            throw std::invalid_argument("argument above maximum available memory");

        auto begin = memory.begin() + t_strt;
        auto end = memory.begin() + t_strt + t_size;
        std::vector<unsigned char> answer;
        answer.insert(answer.begin(), begin, end);
        return answer;
    }



    void Table::write(size_t t_strt, size_t t_size, std::vector<unsigned char> t_vec) noexcept(false) {
        if(t_size > max_size - t_strt)
            throw std::invalid_argument("value too big to write");
        for(size_t i = t_strt; i < t_strt + t_size; ++i){
            memory[i] = t_vec[i - t_strt];
        }
    }


}