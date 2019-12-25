#include <functional>
#include "manager.h"


using namespace manager;


void increment_values(Table& table, DivSeg* ds){
    for(size_t i = 0; i < 25; ++i){
        ds->set_single_instance(std::ref(table), 0, i);
        std::cout << "Placed: " << ds->get_single_instance(table, 0) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

void zeroed_values(Table& table, DivSeg* ds){
    for(size_t x = 0; x < 20; ++x){
        ds->set_single_instance(std::ref(table), 0, x + 100);
        std::cout << "Replaced: " << x + 100 << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


void create_values(Table& table, Program& pr){

}

void delete_values(Table& table, Program& pr){

}


int main() {
    Table table;
    Program program1(&table, 250, "testfile1");
    Program program2(&table, 250, "testfile2");
    DivSeg* ds = dynamic_cast<DivSeg*>(program1.request_memory(1, 6, DivSeg_ID, "test"));
    program2.add_entity(ds);
    std::thread placer(increment_values, std::ref(table), std::ref(ds));
    std::thread reader(zeroed_values, std::ref(table), std::ref(ds));
    reader.join();
    placer.join();
    return 0;
}