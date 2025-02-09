#ifndef SEED_POOL_PROC_HPP
#define SEED_POOL_PROC_HPP

#include "seed_pool_thread.hpp"
#include <cstring>
#include <vector>
#include <memory>
#include <algorithm>

/* @todo
1. 文件读入初始种子 （暂时要求用户正式使用seedpool前 先执行add_seed一下）
*/
namespace SeedPoolServicer
{

class SeedPoolProc
{
public:
  SeedPoolProc() {};

  template <typename T = SeedPoolThread>
  bool createSeedPool( // 创建种子池
    const std::string & seed_pool_name, // 种子池名称
    const bool & ave_possibility,       // 是否平均概率
    const std::string init_seed_folder_path = "" // 初始种子池
  );

  void launch();   // launch all seed_pool_threads
  void shutdown(); // shutdown all seed_pool_threads

  ~SeedPoolProc() {};

private:
  std::vector<std::string> seed_pool_names_;   // name of each seed_pool
  std::vector<SeedPoolThread::SharedPtr> seed_pools_;   // each seed_pool  // 支持多态

};




}


#endif
