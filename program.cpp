//
// Created by antony on 11/19/19.
//

#include "manager.h"


namespace manager{

    std::string Program::menu[] = {"0. Quit this menu",
                                   "1. Request memory",
                                   "2. Free memory",
                                   "3. Use div segments",
                                   "4. Show all memory info",
                                   "5. Show div segs",
                                   "6. Calculate total memory used"};



    int Program::run(std::iostream& strm, Table& table) {
        int rc;
        while(rc = answer(strm, menu, menus)){
            (this->*fptr[rc])(table);
        }
        std::cout << "That's all. Bye!" << std::endl;
        return 0;
    }



    Program::Program() : memory_quota(50) {
        entities = nullptr;
        file_address = "default";
        fptr[0] = nullptr;
        fptr[1] = &Program::d_request_memory;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }



    Program::Program(unsigned int t_mem, std::string t_addr) : memory_quota(t_mem) {
        file_address = std::move(t_addr);
        entities = nullptr;
        fptr[0] = nullptr;
        fptr[1] = &Program::d_request_memory;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }



    Entity* Program::request_memory(unsigned int t_amount, Entity_ID t_id, Table& table) {
        Entity* ptr = nullptr;
        try{
            ptr = table.allocate_memory(t_amount, t_id);
        }
        catch(std::runtime_error &rt){
            throw;
        }
        catch(...){
            
        }



        return ptr;
    }

}