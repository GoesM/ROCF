#include "seed_pool_server.hpp"


namespace SeedPoolServicer
{

void SeedPoolServer::launch()
{
    open_shared_mem();
    launch_fork();
    while( *signal_flag_!= SIGNAL::ACTIVATED_STATE)  usleep(10000);   // 0.01s
    return ; 
}

void SeedPoolServer::shutdown(uint64_t restore_for_future_num)
{
    if(signal_flag_ == nullptr){
        std::cerr << " Please launch() before shutdown()." << std::endl;
        return ; 
    }
    // send signal to shutdown
    *signal_flag_ = SIGNAL::REQUEST_SHUTDOWN;
    while( *signal_flag_ != SIGNAL::SHUTDOWN_STATE)
    {
        usleep(10000);   // 0.01s
    }
    // send signal to restore_for_future
    *signal_flag_ = restore_for_future_num;
    while( *signal_flag_ != SIGNAL::CLEANUP_STATE)
    {
        usleep(10000);   // 0.01s
    }
    // clean up 
    close_shared_mem();
}



void SeedPoolServer::launch_fork()
{
    pid_t pid = fork();
    if (pid == 0) {  // 子进程
        file_seed_pool_server.launch();
        regular_seed_pool_server.launch();
        *signal_flag_ = SIGNAL::ACTIVATED_STATE;
        while( *signal_flag_ != SIGNAL::REQUEST_SHUTDOWN)
        {
            usleep(10000);   // 0.01s
        }
        file_seed_pool_server.shutdown();
        regular_seed_pool_server.shutdown();
        *signal_flag_ = SIGNAL::SHUTDOWN_STATE;  // call back signal

        while( SIGNAL::isSignal(*signal_flag_) )
        {
            usleep(10000);   // 0.01s
        }
        file_seed_pool_server.restoreForFuture(*signal_flag_);
        file_seed_pool_server.cleanTmpSeedPool();
        *signal_flag_ = SIGNAL::CLEANUP_STATE;  // call back signal

        exit(0);
    } else if (pid > 0) {
        // 父进程
    } else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    return ; 
}


}