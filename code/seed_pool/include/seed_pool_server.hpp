#ifndef SEED_POOL_SERVER_HPP
#define SEED_POOL_SERVER_HPP

#include "seed_pool_proc.hpp"
#include "file_seed_pool_proc.hpp"
#include <sys/mman.h>


namespace SeedPoolServicer
{

namespace SIGNAL
{
    const int EMPTY = -999;
    const int REQUEST_SHUTDOWN = -1;
    const int ACTIVATED_STATE = -3;
    const int SHUTDOWN_STATE = -4;
    const int CLEANUP_STATE = -5;
    bool isSignal(const int & num) {return num<0;}
}



class SeedPoolServer
{
public:
    SeedPoolServer(){};
    ~SeedPoolServer(){};

    // 创建file种子池
    template <typename T = FileSeedPoolThread>
    void createFileSeedPool(
        const std::string & seed_pool_name, // 种子池名称
        const bool & ave_possibility,       // 是否平均概率
        const std::string init_seed_folder_path = "" // 初始种子池
    ) {file_seed_pool_server.createSeedPool<T>(seed_pool_name,ave_possibility,init_seed_folder_path);}

    // 创建regular种子池
    template <typename T = SeedPoolThread>
    void createSeedPool( 
        const std::string & seed_pool_name, // 种子池名称
        const bool & ave_possibility,       // 是否平均概率
        const std::string init_seed_folder_path = "" // 初始种子池
    ) {regular_seed_pool_server.createSeedPool<T>(seed_pool_name,ave_possibility,init_seed_folder_path);}

    void launch();
    void shutdown(uint64_t restore_for_future);

private:
    FileSeedPoolProc file_seed_pool_server;
    SeedPoolProc regular_seed_pool_server;

private:
    int *signal_flag_{nullptr};
    void launch_fork();
    inline void open_shared_mem()
    {
        signal_flag_ = (int *)mmap(
            NULL, sizeof(int), 
            PROT_READ | PROT_WRITE,  // 读写权限。
            MAP_SHARED | MAP_ANONYMOUS,  // 共享内存，匿名映射
            -1, 0);
        if( signal_flag_ == MAP_FAILED){
            perror("mmap failed");
            exit(EXIT_FAILURE);
        }
        *signal_flag_ = SIGNAL::EMPTY;
        return ; 
    }
    inline void close_shared_mem()
    {
        munmap(signal_flag_, sizeof(int));
        signal_flag_ = nullptr;
        return ; 
    }

};


}


#endif