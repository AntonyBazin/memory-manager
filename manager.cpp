//
// Created by antony on 10/29/19.
//

#include "manager.h"

namespace manager{

    Table &Table::cmd_Table() {
        static Table instance;
        return instance;
    }

}