#include "configer.hpp"



int main()
{
    CONFIGER::set("DurationEachround",  40);
    int val = CONFIGER::read<int>("DurationEachround");
    std::cout << "read configuration DurationEachround:" << val << std::endl;
    
    std::string va = CONFIGER::read_from_yamlfile<std::string>("../test/test_log.yaml", "FUZZER_NAME");
    std::cout << "read configuration FUZZER_NAME: " << va << std::endl;

    CONFIGER::set_from_yamlfile("../test/test_log.yaml");
    bool flag = CONFIGER::read<bool>("if_flag_log_clean");
    std::cout << "configuration: if_flag_log_clean = " << flag << std::endl;

}