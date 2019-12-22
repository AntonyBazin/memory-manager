//
// Created by antony on 12/10/19.
//

#include "manager.h"


namespace manager{



    App::App() {
        table = new Table();
    }



    int App::command() {
        int i = 0;
        std::cout << "Which program to run?" << std::endl;
        list_programs();
        std::cin >> i;
        try{
            programs.at(i)->run();
        } catch(std::exception& ex){
            std::cerr << ex.what() << std::endl;
            return 1;
        }
        return 1;
    }



    void App::create_program() {
        size_t q;
        std::string name;
        std::cout << "Enter program's source file's address: ";
        std::cin >> name;
        std::cout << std::endl << "Enter the program's memory quota: ";
        std::cin >> q;
        auto pr = new Program(table, q, name);
        programs.push_back(pr);
    }



    void App::run() {
        int rc = 1;
        while(rc != 0){
            std::cout << "What to do?" << std::endl;
            std::cout << "0 - quit;" << std::endl
                      << "1 - add program;" << std::endl
                      << "2 - run program;" << std::endl
                      << "3 - add a DivSeg to a program;" << std::endl
                      << "4 - list programs." << std::endl;
            std::cout << "Input number: ";
            std::cin >> rc;
            std::cout << std::endl;
            switch(rc){
                case 0:
                    rc = 0;
                    break;
                case 1:
                    create_program();
                    break;
                case 2:
                    command();
                    break;
                case 3:
                    add_ds();
                    break;
                case 4:
                    list_programs();
                    break;
                default:
                    std::cout << "Unexpected input. Try again." << std::endl;
                    rc = 0;
            }
            std::cout << std::endl;
        }
        std::cout << "That's all. Bye!" << std::endl;
    }



    void App::add_ds() {
        size_t i = 0, j = 0;
        std::vector<Entity*> ds_vec = {};
        std::cout << "Select a program to add to:" << std::endl;
        for(auto pr = programs.begin(); pr != programs.end(); ++pr, ++i){
            std::cout << i << " " << (*pr)->get_address() << std::endl;
        }
        std::cin >> i;
        if(i >= programs.size()){
            std::cout << "There is no such program. Try again, please." << std::endl;
            return;
        }
        std::cout << "Select an available DivSeg:" << std::endl;
        for(auto program : programs){
            auto dss = (*program).get_div_segs();   // form a vector of DivSeg
            for(; j < dss.size(); ++j){
                ds_vec.push_back(dss.at(j));
            }
        }

        if(ds_vec.empty()){
            std::cout << "No Div Segments available!" << std::endl;
            return;
        } else{
            for(j = 0; j < ds_vec.size(); ++j){
                std::cout << j << ") " << ds_vec.at(j)->get_name() << std::endl;
            }
        }

        std::cin >> j;
        try{
            programs.at(i)->add_existing_DivSeg(ds_vec.at(j));
        } catch(std::out_of_range& oo){
            std::cout << "No such Div Seg: " << oo.what() << std::endl;
        } catch(std::domain_error& dm){
            std::cerr << "Cannot add a DivSeg: " << dm.what() << std::endl;
        } catch(std::exception& ex){
            std::cerr << "Unexpected error: " << ex.what() << std::endl;
        }
    }



    void App::list_programs() {
        if(programs.empty()){
            std::cout << "No programs detected." << std::endl;
        } else{
            for(int i = 0; i < programs.size(); ++i){
                std::cout << i << ") " << programs.at(i)->get_address() << std::endl;
            }
        }
    }



    App::~App() {
        for(auto program : programs){
            delete program;
        }
        programs.clear();
        delete table;
    }


}