#include "seed_pool_proc.hpp"
int main()
{
    std::cout << "---------seed_pool_server.---------" << std::endl;
    
    std::cout << "输入初始种子池目录" << std::endl; 
    std::string targetFolderPath;
    std::cin>> targetFolderPath ;

    SeedPoolServicer::SeedPoolProc seed_pool_manager;
    
    // create 多seed_pool， 这里的seed_pool类型也可以是由SeedPoolThread改写的子类。
    using RegularSeedPool = SeedPoolServicer::SeedPoolThread;
    seed_pool_manager.createSeedPool
        <RegularSeedPool>                               // 创建的种子池类型（保证是SeedPoolThread的子类即可）
        ("seed_pool_1", true, targetFolderPath.c_str()); // 种子池名称，是否平均概率，初始种子池路径
    seed_pool_manager.createSeedPool
        <RegularSeedPool>
        ("seed_pool_2", false, targetFolderPath.c_str());
    // launch 
    seed_pool_manager.launch();
    // keep working until input something
    std::cout << "input anything to shutdown " << std::endl;
    std::string a; std::cin>>a;
    
    // exit
    seed_pool_manager.shutdown();

    return 0;
}