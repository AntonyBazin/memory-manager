#include <functional>
#include "manager.h"


using namespace manager;

void create_values(Table& table, Program& program){
    for(unsigned long long i = 0; i < 21; ++i){
        Value* val = dynamic_cast<Value*>(program.request_memory(1, 8, Value_ID, "test"));
        program.add_entity(val);
        val->set_instance(std::ref(table), i);
        std::cout << "Placed: " << val->get_instance(table) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

void destroy_values(Table& table, Program& program){
    while(true){
        try{
            const auto val = dynamic_cast<const Value*>(program.get_entity(0));
            std::cout << "Fetched: " << val->get_instance(table) << std::endl;
            program.free_entity(0);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } catch(std::exception& ex){
            break;
        }
    }
}


int main() {
    Table table;
    Program program(&table, 400, "testfile");
    std::thread placer(create_values, std::ref(table), std::ref(program));
    std::thread reader(destroy_values, std::ref(table), std::ref(program));
    reader.join();
    placer.join();
    return 0;
}