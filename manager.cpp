//
// Created by antony on 10/29/19.
//

#include "manager.h"

namespace manager{

    Table::Table() {
        current_size = 0;
        memory = new unsigned char[max_size];
        free_blocks = {};
        free_blocks.emplace_back(0, max_size, false);
    }

    void Table::defragmentation() {

    }

    void Table::mark_free(unsigned int t_strt, unsigned int t_size) {

    }


}