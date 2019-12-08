//
// Created by antony on 11/19/19.
//

#include "manager.h"


namespace manager{

    std::string Program::menu[] = {"0. Stop controlling this program",
                                   "1. Request memory for an entity",
                                   "2. Free memory",
                                   "3. Work with entity",
                                   "4. Show all memory info",
                                   "5. Show div segments",
                                   "6. Calculate total memory used"};

    const int Program::menus = sizeof(menu)/sizeof(menu[0]);


    int Program::run() {
        int rc;
        while((rc = answer(menus, menu))){
            (this->*fptr[rc])();
        }
        std::cout << "That's all. Bye!" << std::endl;
        return 0;
    }



    Program::Program(Table& tbl, size_t t_mem, std::string t_addr) : memory_quota(t_mem) {
        table = tbl;
        file_address = std::move(t_addr);
        entities = {};
        fptr[0] = nullptr;
        fptr[1] = &Program::d_create_entity;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }



    Entity* Program::request_memory(size_t t_amount,
            Entity_ID e_id,
            const std::string& t_name) noexcept(false){

        Unit rc;
        Entity* ptr = nullptr;
        try{
            rc = table.allocate_memory(t_amount);
            ptr = Entity::generate_Entity(e_id, e_id, t_name);
            ptr->set_pos(rc);
        }
        catch(...){
            throw;
        }
        return ptr;
    }



    void Program::add_entity(Entity* ent) {
        entities.emplace_back(ent);
        ent->increment_refs();
    }



    void Program::free_entity(size_t t_index) noexcept(false) {
        Unit pos = entities.at(t_index)->get_pos();
        auto mark = entities.begin() + t_index;
        (*mark)->decrement_refs();
        if(!(*mark)->get_refs_count()){  // check whether entity is now free
            delete (*mark);  // if it has no refs any more than delete it
            table.mark_free(pos.starter_address, pos.size); // and mark as free
        }
        entities.erase(mark); // delete from this programs entities anyway
    }



    size_t Program::memory_used() const {
        size_t sz = 0;
        std::for_each(entities.begin(),
                entities.end(),
                [&sz](Entity* en) { sz += en->get_size(); });
        return sz;
    }



    void Program::free_all_memory() noexcept{
        auto vec_it = entities.begin();
        for(; vec_it != entities.end(); ++vec_it){
            Unit current_pos = (*vec_it)->get_pos();
            (*vec_it)->decrement_refs();
            if(!(*vec_it)->get_refs_count()){
                table.mark_free(current_pos.starter_address, current_pos.size);
                delete (*vec_it);
            }
        }
        entities.clear();
    }



    Program::~Program() {
        if(!entities.empty()){
            free_all_memory();
        }
    }



    Program::Program(const Program& program) : memory_quota(program.memory_quota) {
        this->file_address = program.file_address;
        this->table  = program.table;
        auto it = program.entities.cbegin();  // this is a const iterator
        for(; it != program.entities.cend(); ++it){
            this->entities.push_back((*it)->clone());
        }
        fptr[0] = nullptr;
        fptr[1] = &Program::d_create_entity;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }



    Program::Program(Program&& program) noexcept : memory_quota(program.memory_quota){
        this->file_address = program.file_address;
        this->table  = program.table;
        std::move(program.entities.begin(),
                program.entities.end(),
                this->entities.begin());
        program.entities.clear();
        fptr[0] = nullptr;
        fptr[1] = &Program::d_create_entity;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }



    void Program::add_existing_DivSeg(Entity *ent) noexcept(false){
        if(ent->get_entity_id() != DivSeg_ID)
            throw std::domain_error("received a non-DivSeg on adding a DivSeg");
        auto d_ptr = dynamic_cast<DivSeg*>(ent);
        d_ptr->add_program(*this);
        d_ptr->increment_refs();
        entities.push_back(d_ptr);
    }



    void Program::refuse_div_seg(Entity* ent) noexcept(false) {
        if(ent->get_entity_id() != DivSeg_ID)
            throw std::domain_error("received a non-DivSeg on refusing a DivSeg");
        auto d_ptr = dynamic_cast<DivSeg*>(ent);
        d_ptr->erase_program(*this);
        d_ptr->decrement_refs();
        entities.erase(std::find(entities.begin(),
                entities.end(), d_ptr));
        if(!(d_ptr->get_refs_count())){
            delete d_ptr;
        }
    }



    std::vector<Entity *> Program::get_div_segs() noexcept {
        auto iter = entities.begin();
        std::vector<Entity*> res = {};
        for(; iter != entities.end(); ++iter){
            if((*iter)->get_entity_id() == DivSeg_ID){
                res.push_back((*iter));
            }
        }
        return res;
    }



    std::ostream& Program::show_all(std::ostream& os) const {
        for(auto entity : entities){
            entity->show(table, os);
        }
        return os;
    }



    int Program::d_create_entity() {
        size_t rc;   // deciding the type of the entity
        size_t sz;  // for size of 1 element
        size_t amount;  // for array-based classes
        size_t index;  // for links
        std::string new_name;  // for the name
        Entity* ptr;
        std::cout << "Enter the parameters of the new entity:" << std::endl;
        std::cout << "Choose the type of the entity:" << std::endl;
        std::cout << "1 - single value,\n2 - array,\n3 - divseg,\n4 - link.";
        std::cin >> rc;
        std::cout << "Enter the name of the entity: ";
        std::cin >> new_name;

        switch(rc){
            case 1:
                std::cout << "Enter the size of the value: ";
                std::cin >> sz;
                ptr = request_memory(sz, Value_ID, new_name);
                add_entity(ptr);
                break;
            case 2:
                std::cout << "Enter the size of 1 array element: ";
                std::cin >> sz;
                std::cout << "Enter the length of the array";
                std::cin >> amount;
                ptr = request_memory((sz*amount), Array_ID, new_name);
                add_entity(ptr);
                break;
            case 3:
                std::cout << "Enter the size of 1 divseg array element: ";
                std::cin >> sz;
                std::cout << "Enter the length of the divseg array";
                std::cin >> amount;
                ptr = request_memory((sz*amount), DivSeg_ID, new_name);
                add_entity(ptr);
                break;
            case 4:
                std::cout << "Enter the number of the existing entity"
                          << std::endl << " to create a link to: ";
                std::cin >> index;
                try{
                    ptr = entities.at(index)->create_link(new_name);
                } catch(std::exception& ex){
                    std::cout << ex.what();
                    return 0;
                }
                add_entity(ptr);
                break;
            default:
                break;
        }
        return 1;
    }



    int Program::d_free_memory() {
        size_t index;
        std::cout << "Enter the number of the entity to free: ";
        std::cin >> index;
        try{
            free_entity(index);
        } catch(std::exception& ex){
            std::cout << ex.what();
            return 0;
        }
        return 1;
    }




}