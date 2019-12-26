#include "manager.h"


using namespace manager;


void increment_values(Table& table, DivSeg* ds){
    for(size_t i = 0; i < 10; ++i){
        unsigned long long res = ds->get_single_instance(table, 0);
        std::string buffer = res > 100 ? "" : " ";
        std::cout << "From    zero: | " << buffer << res << buffer;
        ds->set_single_instance(std::ref(table), 0, i);
        std::cout << "| " << ds->get_single_instance(table, 0) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void replace_values(Table& table, DivSeg* ds){
    for(size_t x = 0; x < 15; ++x){
        unsigned long long res = ds->get_single_instance(table, 0);
        std::string buffer = res > 100 ? "" : " ";
        std::cout << "From hundred: | " << buffer << res << buffer;
        ds->set_single_instance(std::ref(table), 0, x + 100);
        std::cout << "| " << ds->get_single_instance(table, 0) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void create_values(Table& table, Program& program){
    for(unsigned long long i = 0; i < 10; ++i){
        DivSeg* ds = dynamic_cast<DivSeg*>(program.request_memory(1, 6, DivSeg_ID, "test"));
        program.add_entity(ds);
        ds->set_single_instance(std::ref(table), 0, i);
        std::cout << "Placed: " << ds->get_single_instance(table, 0) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

void run_sets(Table& table) {
    Program program1(&table, 50, "test1");
    Program program2(&table, 50, "test2");
    Program writer(&table, 150, "test3");
    DivSeg* ds = dynamic_cast<DivSeg*>(program1.request_memory(1, 6, DivSeg_ID, "test"));
    program2.add_entity(ds);
    std::cout << std::endl << "  THREAD:     |READ| WRITE" << std::endl << std::endl;
    std::thread increment(increment_values, std::ref(table), std::ref(ds));
    std::thread decrement(replace_values, std::ref(table), std::ref(ds));
    //std::thread placer(create_values, std::ref(table), std::ref(writer));
    increment.join();
    decrement.join();

    //placer.join();
}


int main(){
    Table table;
    run_sets(table);
    return 0;
}