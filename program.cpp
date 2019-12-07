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
    const int Program::menus = sizeof(menu)/sizeof(menu[0]);


    int Program::run() {
        int rc;
        while((rc = answer(menus, menu))){
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


    Entity* Program::request_memory(size_t t_amount, Type_ID t_id,
            Entity_ID e_id, const std::string& t_name) noexcept(false) {

        Unit rc;
        Entity* ptr = nullptr;
        try{
            rc = table.allocate_memory(t_amount, e_id);
            ptr = Entity::generate_Entity(e_id, t_id, t_name);
            ptr->set_pos(rc);
        }
        catch(...){
            throw;
        }
        entities.emplace_back(ptr);
        ptr->increment_refs();
        return ptr;
    }



    void Program::free_entity(size_t t_index) noexcept(false) {
        Unit pos = entities.at(t_index)->get_pos();
        auto mark = entities.begin() + t_index;
        (*mark)->decrement_refs();
        if(!(*mark)->refs_count()){  // check whether entity is now free
            delete (*mark);  // if it has no refs any more than delete it
            table.mark_free(pos.starter_address, pos.size); // and mark as free
        }
        entities.erase(mark); // delete from this programs entities anyway
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
            (*vec_it)->decrement_refs();
            if(!(*vec_it)->refs_count()){
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



    Program Program::clone() {
        Program clone(*this);
        return clone;
    }



    Program::Program(const Program& program) : memory_quota(program.memory_quota) {
        this->file_address = program.file_address;
        this->table  = program.table;
        auto it = program.entities.cbegin();  // this is a const iterator
        for(; it != program.entities.cend(); ++it){
            this->entities.push_back(Entity::generate_Entity((*it)->get_entity_id(),
                    (*it)->get_type_id(), (*it)->get_name()));
        }
        fptr[0] = nullptr;
        fptr[1] = &Program::d_request_memory;
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
        /*auto it = program.entities.begin();  // this is a const iterator
        for(; it != program.entities.end(); ++it){
            this->entities.push_back(*it);
        }*/
        program.entities.clear();
        fptr[0] = nullptr;
        fptr[1] = &Program::d_request_memory;
        fptr[2] = &Program::d_free_memory;
        fptr[3] = &Program::d_use_divsegs;
        fptr[4] = &Program::d_show_all;
        fptr[5] = &Program::d_show_divsegs;
        fptr[6] = &Program::d_calc_memory;
    }



    void Program::add_existing_DivSeg(Entity *ent) noexcept(false){
        if(ent->get_entity_id() != DivSeg_ID)
            throw std::domain_error("received a non-DivSeg on adding a DivSeg");
        ent->add_program(*this);
        ent->increment_refs();
        entities.push_back(ent);
    }



    void Program::refuse_div_seg(Entity* ent) noexcept(false) {
        if(ent->get_entity_id() != DivSeg_ID)
            throw std::domain_error("received a non-DivSeg on refusing a DivSeg");
        ent->erase_program(*this);
        ent->decrement_refs();
        if(!(ent->refs_count())){
            delete ent;
        }
        entities.erase(std::find(entities.begin(), entities.end(), ent));
    }



    std::vector<Entity *> Program::get_div_segs() const noexcept {
        auto iter = entities.begin();
        std::vector<Entity*> res = {};
        for(; iter != entities.end(); ++iter){
            if((*iter)->get_entity_id() == DivSeg_ID){
                res.push_back((*iter));
            }
        }
        return res;
    }

    std::ostream &Program::show_all(std::ostream& os) const {
        auto iter = entities.begin();
        os << "List of variables:\n" << std::endl;
        for(; iter != entities.end(); ++iter){
            switch((*iter)->get_type_id()){
                case CHAR:
                    os << "char ";
                    break;
                case INT:
                    os << "int ";
                    break;
                case LONG:
                    os << "long ";
                    break;
                case LONGLONG:
                    os << "long long ";
                    break;
                case FLOAT:
                    os << "float ";
                    break;
                case DOUBLE:
                    os << "double ";
                    break;
                case LONGDOUBLE:
                    os << "long double ";
                    break;
                default:
                    os << "error ";
            }

            switch((*iter)->get_entity_id()){
                case Value_ID:
                    os << "Value ";
                    break;

                case Array_ID:
                    os << "Array ";
                    break;

                case DivSeg_ID:
                    os << "DivSeg ";
                    break;

                default:
                    os << "Error_ent ";
            }
            os << (*iter)->get_name() << std::endl;

        }
        return os;
    }


}