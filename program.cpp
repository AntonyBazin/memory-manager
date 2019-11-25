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
        entities = {};
        file_address = "default";
        fptr[0] = nullptr;
        fptr[1] = &Program::d_request_memory;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }



    Program::Program(size_t t_mem, std::string t_addr) : memory_quota(t_mem) {
        file_address = std::move(t_addr);
        entities = {};
        fptr[0] = nullptr;
        fptr[1] = &Program::d_request_memory;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }



    Entity* Program::request_memory(size_t t_amount, Entity_ID t_id, Table& table) noexcept(false) {
        Entity* ptr = nullptr;
        try{
            ptr = table.allocate_memory(t_amount, t_id);
        }
        catch(...){
            throw;
        }
        entities.emplace_back(ptr);
        return ptr;
    }



    void Program::free_entity(size_t t_index) noexcept(false) {
        if(entities.at(t_index)) delete entities.at(t_index);
        auto mark = entities.begin() + t_index;
        entities.erase(mark);
    }



    void Program::refuse_divseg(Entity* ptr) noexcept(false) {
        if(typeid(*ptr) != typeid(DivSeg))
            throw(std::invalid_argument("not a divseg argument"));
        auto dptr = dynamic_cast<DivSeg*>(ptr);
        dptr->erase_one(this);
        auto mark = std::find(entities.begin(), entities.end(), ptr);
        entities.erase(mark);
    }

    size_t Program::get_memory_used() const {
        size_t sz = 0;
        std::for_each(entities.begin(),
                entities.end(),
                [&sz](Entity* en) { sz += en->memory_used(); });
        return sz;
    }


}