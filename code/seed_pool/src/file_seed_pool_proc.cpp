#include "file_seed_pool_proc.hpp"


namespace SeedPoolServicer
{
template bool FileSeedPoolProc::createSeedPool(
  const std::string & seed_pool_name,
  const bool & ave_possibility,
  const std::string seed_folder_path);

template <typename T = FileSeedPoolThread>
bool FileSeedPoolProc::createSeedPool(
  const std::string & seed_pool_name,
  const bool & ave_possibility,
  const std::string seed_folder_path)
{
    static_assert(std::is_base_of<FileSeedPoolThread, T>::value, "T must inherit from FileSeedPoolThread");

    // check 重名
    auto it = std::find(seed_pool_names_.begin(), seed_pool_names_.end(), seed_pool_name);
    if (it != seed_pool_names_.end()) {  
        perror("you have create another seed pool with same name. change the name of this one.");
        return false;
    }
    // create & add in vector
    std::shared_ptr<FileSeedPoolThread> seed_pool_ptr = 
        std::make_shared<T>(seed_pool_name, ave_possibility);
    seed_pools_.push_back(seed_pool_ptr);   // use Ptr for record. (avoid freed just after constructor)
    seed_pool_names_.push_back(seed_pool_name);

    // 初始化种子池
    if (!seed_folder_path.empty()) {
        seed_pool_ptr->init_seeds(seed_folder_path);
    }
    return true;
}


void FileSeedPoolProc::launch()
{
    // launch all SeedPoolThread
    for (auto & seed_pool : seed_pools_) {
        seed_pool->launch();
    }
}

void FileSeedPoolProc::shutdown()
{
    // shutdown all SeedPoolThread
    for (auto & seed_pool : seed_pools_) {
        seed_pool->shutdown();
    }
}


}
