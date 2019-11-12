//
// Created by antony on 10/29/19.
//

#include "manager.h"

namespace manager{

    Table &Table::cmd_Table() {
        static Table instance;
        return instance;
    }

    Table::Table() {
        current_size = 0;
        memory = new unsigned char[300];
        free_blocks_count = 1;
        free_blocks = new Unit(0, 300, false);
    }

    void Table::defragmentation() {
        for(unsigned int i = 1; i < free_blocks_count; ++i){
            if(free_blocks[i].starter_address ==
            free_blocks[i-1].starter_address + free_blocks[i-1].size){
                Unit* new_arr = new Unit[--free_blocks_count];
                for(unsigned int j = 0; j < i - 1; ++j){
                    new_arr[j] = free_blocks[j];
                }
                new_arr[i - 1] = Unit(free_blocks[i-1].starter_address,
                        free_blocks[i - 1].size + free_blocks[i].size,
                        false);
                for(unsigned int j = i + 1; j < free_blocks_count + 1; ++j){
                    new_arr[j - 1] = free_blocks[j];
                }
                --i;   // to check this and next block again
                delete [] free_blocks;
                free_blocks = new_arr;
            }
        }
    }

    void Table::mark_free(unsigned int t_strt, unsigned int t_size) {
        if(t_strt >= max_size)
            throw std::runtime_error("starter address higher than maximum size");
        for(unsigned int i = 0; i < free_blocks_count; ++i){
            if(free_blocks[i].starter_address <= t_strt + t_size &&
            free_blocks[i].starter_address + free_blocks[i].size >= t_strt + t_size)
                throw std::runtime_error("block already marked as free");
            if(free_blocks[i].starter_address > t_strt + t_size) break;  // for optimization
        }
        Unit* new_arr = new Unit[free_blocks_count + 1];

        unsigned int i = free_blocks_count;
        for(; i >= 0 && free_blocks[i].starter_address > t_strt; --i){
            new_arr[i + 1] = free_blocks[i];
        }
        new_arr[i + 1] = Unit(t_strt, t_size, false);
        for(; i > 0; --i){
            new_arr[i] = free_blocks[i];
        }
        new_arr[0] = free_blocks[0];
        delete [] free_blocks;
        free_blocks = new_arr;
    }


}