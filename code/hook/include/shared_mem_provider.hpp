#include "hook_def.hpp"
#include <yaml-cpp/yaml.h>

#ifndef SHARED_MEM_PROVIDER_HPP
#define SHARED_MEM_PROVIDER_HPP


class SharedMemProvider{
public:
    SharedMemProvider(){};
    ~SharedMemProvider(){};

public: 
    void open();
    void close();
    void update(YAML::Node & delayer_queue){
        initialize_shared_memory(delayer_queue);
    }

private:
    key_t key = ftok("/tmp/shared_memory_ROCF", 'G');
    int shmid;
    SharedDataROCF * shared_data{nullptr}; 

private:
    void initialize_shared_memory(YAML::Node & delayer_queue);
};


#endif