#ifndef FILE_SEED_POOL_PROC_HPP
#define FILE_SEED_POOL_PROC_HPP

#include "file_seed_pool_thread.hpp"
#include "seed_pool_proc.hpp"
#include <cstring>
#include <vector>
#include <memory>
#include <algorithm>

/* @todo
1. 文件读入初始种子 （暂时要求用户正式使用seedpool前 先执行add_seed一下）
*/
namespace SeedPoolServicer
{

class FileSeedPoolProc
{
public:
    FileSeedPoolProc(){};
    // create a seed pool without init
    template <typename T = FileSeedPoolThread>
    bool createSeedPool( // 创建种子池
        const std::string & seed_pool_name, // 种子池名称
        const bool & ave_possibility,       // 是否平均概率
        const std::string init_seed_folder_path = "" // 初始种子池
    );

    void launch();   // launch all seed_pool_threads
    void shutdown(); // shutdown all seed_pool_threads

    void restoreForFuture(int num = -1) // restore into init_seed_pool
    {
        for (const auto & seed_pool_ptr : seed_pools_) 
        {
            seed_pool_ptr->restore_for_future(num);
        }
        return;
    }
    void cleanTmpSeedPool() // clean tmp_seed_pool
    {    
        for(const auto & seed_pool : seed_pools_)
            seed_pool->clean_up_tmp_seeds();
    }

    ~FileSeedPoolProc(){};

private:
   std::vector<std::string> seed_pool_names_;   // name of each seed_pool
   std::vector<FileSeedPoolThread::SharedPtr> seed_pools_;   // each seed_pool  // 支持多态

};



}


#endif
