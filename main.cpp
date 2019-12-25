#include "manager.h"


using namespace manager;


void increment_values(Table& table, DivSeg* ds){
    for(size_t i = 0; i < 10; ++i){
        ds->set_single_instance(std::ref(table), 0, i);
        std::cout << "From zero: " << ds->get_single_instance(table, 0) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void replace_values(Table& table, DivSeg* ds){
    for(size_t x = 0; x < 15; ++x){
        ds->set_single_instance(std::ref(table), 0, x + 100);
        std::cout << "From hundred: " << x + 100 << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void create_values(Table& table, Program& program){
    for(unsigned long long i = 0; i < 10; ++i){
        DivSeg* ds = dynamic_cast<DivSeg*>(program.request_memory(1, 6, DivSeg_ID, "test"));
        program.add_entity(ds);
        ds->set_single_instance(std::ref(table), 0, i);
        std::cout << "Placed: " << ds->get_single_instance(table, 0) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


int main(){
    Table table;
    Program program1(&table, 50, "testfile1");
    Program program2(&table, 50, "testfile2");
    Program writer(&table, 150, "testfile3");
    DivSeg* ds = dynamic_cast<DivSeg*>(program1.request_memory(1, 6, DivSeg_ID, "test"));
    program2.add_entity(ds);
    std::thread placer(create_values, std::ref(table), std::ref(writer));
    std::thread increment(increment_values, std::ref(table), std::ref(ds));
    std::thread decrement(replace_values, std::ref(table), std::ref(ds));

    increment.join();
    decrement.join();
    placer.join();

    return 0;
}