#include "seed_pool_server.hpp"
#include "seed_pool_user.hpp"
int main()
{
    std::cout << "输入初始种子池目录" << std::endl; 
    std::string targetFolderPath;
    std::cin>> targetFolderPath ;
    
    // config server
    SeedPoolServicer::SeedPoolServer seed_pool_server_;
    seed_pool_server_.createFileSeedPool
        <SeedPoolServicer::FileSeedPoolThread>
        ("seed_pool_1", true, targetFolderPath.c_str());
    seed_pool_server_.createSeedPool
        <SeedPoolServicer::SeedPoolThread>
        ("seed_pool_2", false, targetFolderPath.c_str());
    // launch server
    seed_pool_server_.launch();

        // indirect API
        SeedPoolUser::SeedPoolUser seed_pool_user("seed_pool_1");  // 种子池名称
        std::string picked_seed = seed_pool_user.pickSeed();  // 种子pick： pick一个种子
        std::cout << "pick seed:" << picked_seed << std::endl;
        seed_pool_user.addSeed(picked_seed, 1);  // 种子储存： 种子数据为"Seed_1"， 种子权重为 1  
        
        // direct API
        SeedPoolUser::addSeedInPool("seed_pool_2", picked_seed,4);
        picked_seed = SeedPoolUser::pickSeedFromPool("seed_pool_2");
        std::cout << "pick seed:" << picked_seed << std::endl;


    // shutdown server
    seed_pool_server_.shutdown(1);
    return 0 ; 
}