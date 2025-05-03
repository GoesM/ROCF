#include "seed_pool_proc.hpp"


namespace SeedPoolServicer
{
template bool SeedPoolProc::createSeedPool( // 创建种子池
    const std::string & seed_pool_name, // 种子池名称
    const bool & ave_possibility,       // 是否平均概率
    const std::string init_seed_folder_path = "" // 初始种子池
);

template <typename T = SeedPoolThread>
bool SeedPoolProc::createSeedPool(
  const std::string & seed_pool_name,
  const bool & ave_possibility,
  const std::string init_seed_folder_path)
{
    static_assert(std::is_base_of<SeedPoolThread, T>::value, "T must inherit from SeedPoolThread");
    // check 重名
    auto it = std::find(seed_pool_names_.begin(), seed_pool_names_.end(), seed_pool_name);
    if (it != seed_pool_names_.end()) {
        perror("you have create another seed pool with same name. change the name of this one.");
        return false;
    }

    // create & add in vector
    std::shared_ptr<SeedPoolThread> seed_pool_ptr = 
        std::make_shared<T>(seed_pool_name,ave_possibility);
    seed_pools_.push_back(seed_pool_ptr);   // use Ptr for record. (avoid freed just after constructor)
    seed_pool_names_.push_back(seed_pool_name);

    // 初始化种子池
    if (!init_seed_folder_path.empty()) {
        seed_pool_ptr->init_seeds(init_seed_folder_path);
    }
    return true;
}

void SeedPoolProc::launch()
{
    // launch all SeedPoolThread
    for (auto & seed_pool : seed_pools_) {
        seed_pool->launch();
    }
}

void SeedPoolProc::shutdown()
{
    // shutdown all SeedPoolThread
    for (auto & seed_pool : seed_pools_) {
        seed_pool->shutdown();
    }
}



}
