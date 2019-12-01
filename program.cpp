//
// Created by antony on 11/19/19.
//

#include "manager.h"


namespace manager{

    std::string Program::menu[] = {"0. Stop controlling this program",
                                   "1. Request memory",
                                   "2. Free memory",
                                   "3. Use div segments",
                                   "4. Show all memory info",
                                   "5. Show div segments",
                                   "6. Calculate total memory used"};



    int Program::run() {
        int rc;
        while((rc = answer(menus, menu)){
            (this->*fptr[rc])(table);
        }
        std::cout << "That's all. Bye!" << std::endl;
        return 0;
    }



    Program::Program(Table& tbl, size_t t_mem, std::string t_addr) : memory_quota(t_mem) {
        table = tbl;
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


    template<class T>
    Entity* Program::request_memory(size_t t_amount, Entity_ID t_id) noexcept(false) {
        Unit rc;
        Entity* ptr = nullptr;
        try{
            rc = table.allocate_memory(t_amount, t_id);
            ptr = Entity::create_Entity<T>(t_id);
            ptr->set_pos(rc);
        }
        catch(...){
            throw;
        }
        entities.emplace_back(ptr);
        return ptr;
    }



    void Program::free_entity(size_t t_index) noexcept(false) {
        Unit pos = entities.at(t_index)->get_pos();
        table.mark_free(pos.starter_address, pos.size);
        if(entities.at(t_index)) delete entities.at(t_index);
        auto mark = entities.begin() + t_index;
        entities.erase(mark);
    }


    template<class T>
    void Program::refuse_divseg(Entity* ptr) noexcept(false) {
        if(typeid(*ptr) != typeid(DivSeg<T>))
            throw(std::invalid_argument("not a divseg argument"));
        auto dptr = dynamic_cast<DivSeg<T>*>(ptr);
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



    void Program::free_all_memory() noexcept{
        auto vec_it = entities.begin();
        for(; vec_it != entities.end(); ++vec_it){
            Unit current_pos = (*vec_it)->get_pos();
            table.mark_free(current_pos.starter_address, current_pos.size);
            delete (*vec_it);
        }
        entities.clear();
    }



    Program::~Program() {
        if(!entities.empty()){
            free_all_memory();
        }
    }



    Program Program::clone() {
        Program clone(*this);
        return clone;
    }



    Program::Program(const Program& prog) : memory_quota(prog.memory_quota) {
        this->file_address = prog.file_address;
        this->table  = prog.table;
        auto it = prog.entities.begin();  // this is a const iterator
        for(; it != prog.entities.end(); ++it){
            this->entities.push_back(Entity::create_Entity<>( (*it)->get_id()) );
        }
        fptr[0] = nullptr;
        fptr[1] = &Program::d_request_memory;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }



    Program::Program(Program&& prog) noexcept : memory_quota(prog.memory_quota){
        this->file_address = prog.file_address;
        this->table  = prog.table;
        auto it = prog.entities.begin();  // this is a const iterator
        for(; it != prog.entities.end(); ++it){
            this->entities.push_back(*it);
        }
        prog.entities.clear();
        fptr[0] = nullptr;
        fptr[1] = &Program::d_request_memory;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }


}