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
        used_units_count = 0;
        programs = nullptr;
        programs_amount = 0;
        memory = new char[max_size];
    }
}