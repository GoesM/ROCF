#include "shared_mem_provider.hpp"

void 
SharedMemProvider::open(){
    // 创建共享内存
    int shmid = shmget(key, sizeof(SharedDataROCF), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("provide shared memory failed");
        exit(EXIT_FAILURE);
    }
    // 附加共享内存
    shared_data = (SharedDataROCF *)shmat(shmid, NULL, 0);
    if (shared_data == (SharedDataROCF *)-1) {
        perror("provide shared memory failed");
        exit(EXIT_FAILURE);
    }
    // initial
    YAML::Node ls = YAML::Node();
    initialize_shared_memory(ls);
}

void 
SharedMemProvider::close(){
    shmdt(shared_data);    // detach shared memory
    shmctl(shmid, IPC_RMID, NULL); // clean shared memory
}

void SharedMemProvider::initialize_shared_memory(YAML::Node & delayer_queue) // 初始化共享内存
{
    // delayer
    if(!delayer_queue.IsSequence()) {
        // 将 delay_time 数组中的所有值初始化为 0
        for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
            shared_data->delay_time[i] = 0;
        }
    }
    else{
        std::size_t length = delayer_queue.size();
        // 将 delay_time 数组中的所有值初始化为the value from seed，范围为 0 到 1e9
        for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
            shared_data->delay_time[i] = std::abs(delayer_queue[i%length].as<int>()) % 1000000000 ; // 0~1s范围内 
        }
    }
    return ; 
}
