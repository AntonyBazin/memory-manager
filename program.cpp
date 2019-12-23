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
                                   "5. Show div segments"};

    const int Program::menus = sizeof(menu)/sizeof(menu[0]);


    int Program::run() {
        int rc;
        while((rc = answer(menus, menu))){
            (this->*fptr[rc])();
            std::cout << std::endl;
        }
        return 0;
    }



    Program::Program(Table* tbl, size_t t_mem, std::string t_addr) : memory_quota(t_mem) {
        table = tbl;
        file_address = std::move(t_addr);
        entities = {};
        fptr[0] = nullptr;
        fptr[1] = &Program::d_create_entity;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_entity;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
    }



    Entity* Program::request_memory(size_t t_amount,
            size_t single_val,
            Entity_ID e_id,
            const std::string& t_name) noexcept(false){

        if(t_amount*single_val + memory_used() > memory_quota)
            throw std::runtime_error("memory quota reached for this program");
        if(single_val > sizeof(unsigned long long))
            throw std::domain_error("elements too big!");

        Unit rc;
        Entity* ptr = nullptr;
        try{
            rc = table->allocate_memory(t_amount*single_val);
            ptr = Entity::generate_Entity(e_id, single_val, t_name);
            ptr->set_pos(rc);
        }
        catch(...){
            throw;
        }
        return ptr;
    }



    void Program::add_entity(Entity* ent) noexcept(false) {
        if(std::find(entities.begin(), entities.end(), ent) != entities.end())
            throw std::invalid_argument("Entity already exists in this program!");

        entities.push_back(ent);
        ent->increment_refs();
        if(ent->get_entity_id() == DivSeg_ID){
            auto d_ptr = dynamic_cast<DivSeg*>(ent);
            d_ptr->add_program(this);
        }
    }



    void Program::free_entity(size_t t_index) noexcept(false) {
        Unit pos = entities.at(t_index)->get_pos();
        auto mark = entities.begin() + t_index;
        (*mark)->decrement_refs();
        if((*mark)->get_entity_id() == DivSeg_ID){  // if it is a DivSeg don't forget
            auto d_ptr = dynamic_cast<DivSeg*>((*mark));   // to erase the link to this program
            d_ptr->erase_program(this);
        }
        if(!(*mark)->get_refs_count()){  // check whether entity is now free
            delete (*mark);  // if it has no refs any more than delete it
            table->mark_free(pos.starter_address, pos.size); // and mark as free
        }
        entities.erase(mark); // delete from this programs entities anyway
        check_links(pos);
    }



    size_t Program::memory_used() const {
        size_t sz = 0;
        std::for_each(entities.begin(),
                entities.end(),
                [&sz](Entity* en) { if(en->get_entity_id() != Link_ID){ sz += en->get_size(); } });
        return sz;
    }



    void Program::free_all_memory() noexcept {
        auto vec_it = entities.begin();
        for(; vec_it != entities.end(); ++vec_it){
            Unit current_pos = (*vec_it)->get_pos();
            (*vec_it)->decrement_refs();
            if(!(*vec_it)->get_refs_count()){
                try{
                    table->mark_free(current_pos.starter_address, current_pos.size);
                } catch(...){ }
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
        fptr[3] = &Program::d_use_entity;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
    }



    void Program::add_existing_DivSeg(Entity* ent) noexcept(false){
        if(ent->get_entity_id() != DivSeg_ID)
            throw std::domain_error("received a non-DivSeg on adding a DivSeg");
        if(ent->get_size() + memory_used() > memory_quota)
            throw std::invalid_argument("Received DivSeg is too big");

        auto d_ptr = dynamic_cast<DivSeg*>(ent);
        d_ptr->add_program(this);
        d_ptr->increment_refs();
        entities.push_back(d_ptr);
    }



    std::vector<Entity*> Program::get_div_segs() noexcept {
        auto iter = entities.begin();
        std::vector<Entity*> res = {};
        for(; iter != entities.end(); ++iter){
            if((*iter)->get_entity_id() == DivSeg_ID){
                res.push_back((*iter));
            }
        }
        return res;
    }



    std::ostream& Program::show_all(std::ostream& os) const noexcept {
        for(auto entity : entities){
            entity->show(*table, os);
            os << std::endl;
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

        try{
            switch(rc){
                case 1:
                    std::cout << "Enter the size of the value: ";
                    std::cin >> sz;
                    ptr = request_memory(1, sz, Value_ID, new_name);
                    add_entity(ptr);
                    break;
                case 2:
                    std::cout << "Enter the size of 1 array element: ";
                    std::cin >> sz;
                    std::cout  << std::endl << "Enter the length of the array";
                    std::cin >> amount;
                    ptr = request_memory(amount, sz, Array_ID, new_name);
                    add_entity(ptr);
                    break;
                case 3:
                    std::cout << "Enter the size of 1 divseg array element: ";
                    std::cin >> sz;
                    std::cout << "Enter the length of the divseg array";
                    std::cin >> amount;
                    ptr = request_memory(amount, sz, DivSeg_ID, new_name);
                    add_entity(ptr);
                    break;
                case 4:
                    std::cout << "Registered entities:" << std::endl;
                    for(size_t i = 0; i < entities.size(); ++i){
                        std::cout << i << ") " << entities.at(i)->get_name() << std::endl;
                    }
                    std::cout << "Enter the number of the existing entity"
                              << std::endl << "to create a link to: ";
                    std::cin >> index;
                    try{
                        ptr = entities.at(index)->create_link(new_name);
                    } catch(std::exception& ex){
                        std::cerr << ex.what();
                        return 0;
                    }
                    add_entity(ptr);
                    break;
                default:
                    break;
            }
        } catch(std::exception& ex){
            std::cerr << ex.what() << std::endl;
            return 0;
        }

        return 1;
    }



    int Program::d_free_memory() {
        size_t index;
        std::cout << "Entities: " << std::endl;
        for(size_t i = 0; i < entities.size(); ++i){
            std::cout << i << ") " << entities.at(i)->get_name() << std::endl;
        }
        std::cout << "Enter the number of the entity to free: ";

        std::cin >> index;
        try{
            free_entity(index);
            std::cout << "Freeing successful" << std::endl;
        } catch(std::out_of_range& oo){
            std::cerr << "Incorrect index: " << oo.what() << std::endl;
        } catch(std::exception& ex){
            std::cerr << "Freeing memory: " << ex.what() << std::endl;
            return 0;
        }
        return 1;
    }



    int Program::d_use_entity() {
        size_t index;  // index of the entity
        for(size_t i = 0; i < entities.size(); ++i){
            std::cout << i << ") " << entities.at(i)->get_name() << std::endl;
        }
        std::cout << "Enter the index of entity to use: ";
        std::cin >> index;
        try{
            entities.at(index)->run(*table, std::cout);
        } catch(std::out_of_range& oo){
            std::cerr << "Incorrect index: " << oo.what() << std::endl;
        } catch(std::exception& ex){
            std::cerr << "Use of entity: " << ex.what() << std::endl;
            return 0;
        }
        std::cout << std::endl;
        return 1;
    }



    int Program::d_show_all() {
        std::cout << "Entities amount: " << entities.size() << std::endl;
        std::cout << "Total memory used: " << memory_used() << std::endl;
        std::cout << "Of memory quota: " << memory_quota << std::endl << std::endl;
        show_all(std::cout);
        return 1;
    }



    int Program::d_show_divsegs() {
            try{
                for(auto entity : get_div_segs()){
                    if(entity->get_entity_id() == DivSeg_ID){
                        entity->show(*table, std::cout);
                        dynamic_cast<DivSeg*>(entity)->show_programs(std::cout);
                    }
                }
            } catch(std::exception &ex){
                std::cerr << ex.what() << std::endl;
                return 0;
            }
        return 1;
    }



    int Program::answer(int menus_count, std::string *variants) {
        short ans = 0;
        std::cout << "Choose action: " << std::endl;
        for(int i = 0; i < menus_count; ++i){
            std::cout << variants[i] << std::endl;
        }
        std::cin >> ans;
        return ans;
    }



    Program& Program::operator=(const Program& program) {
        this->file_address = program.file_address;
        this->table  = program.table;
        auto it = program.entities.cbegin();  // this is a const iterator
        for(; it != program.entities.cend(); ++it){
            this->entities.push_back((*it)->clone());
        }
        fptr[0] = nullptr;
        fptr[1] = &Program::d_create_entity;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_entity;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        return *this;
    }



    Program& Program::operator=(Program&& program) noexcept {
        this->file_address = program.file_address;
        this->table  = program.table;
        auto it = program.entities.cbegin();  // this is a const iterator
        for(; it != program.entities.cend(); ++it){
            this->entities.push_back((*it)->clone());
        }
        fptr[0] = nullptr;
        fptr[1] = &Program::d_create_entity;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_entity;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        return *this;
    }



    bool Program::operator==(const Program& pr) {
        if(this->memory_quota != pr.memory_quota)
            return false;
        if(this->entities != pr.entities)
            return false;
        if(this->memory_used() != pr.memory_used())
            return false;
        if(this->file_address != pr.file_address)
            return false;
        return true;
    }



    void Program::check_links(const Unit guard) {
        for(size_t i = 0; i < entities.size(); ++i){
            size_t sz = entities.size();
            if(entities.at(i)->get_entity_id() == Link_ID &&
            guard == (entities.at(i)->get_pos())){
                std::cerr << "Invalid Link: "
                              << entities.at(i)->get_name()
                              << std::endl;
                entities.erase(entities.begin() + i);
                i = -1;
            }
        }
    }

    const Entity* Program::get_entity(size_t index) const noexcept(false) {
        return (entities.at(index));
    }


}