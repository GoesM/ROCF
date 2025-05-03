#include "file_seed_pool_proc.hpp"
int main()
{
    std::cout << "---------file_seed_pool_server test.---------" << std::endl;

    std::cout << "输入初始种子池目录" << std::endl; 
    std::string targetFolderPath;
    std::cin>> targetFolderPath ;

    SeedPoolServicer::FileSeedPoolProc seed_pool_manager;

    using FileSeedPool = SeedPoolServicer::FileSeedPoolThread;
    seed_pool_manager.createSeedPool
        <FileSeedPool>                                   // 创建的种子池类型（保证是FileSeedPoolThread的子类即可）
        ("seed_pool_1", true, targetFolderPath.c_str()); // 种子池名称，是否平均概率，初始种子池路径 
    seed_pool_manager.createSeedPool
        <FileSeedPool>
        ("seed_pool_2", false, targetFolderPath.c_str());
    
    // launch
    seed_pool_manager.launch();
    // keep working until input something
    std::cout << "input anything to shutdown " << std::endl;
    std::string a; std::cin>>a;

    // exit
    seed_pool_manager.shutdown();

    // clean ?
    seed_pool_manager.restoreForFuture(); // 同步到init_seed_pool中
    seed_pool_manager.cleanTmpSeedPool(); // 【推荐】清理临时种子池
    return 0;
}
