#ifndef FILE_SEED_POOL_THREAD_HPP
#define FILE_SEED_POOL_THREAD_HPP

// record file-path
// all seed context is in file
#include "seed_pool_thread.hpp"
#include <chrono>
#include <ctime>

namespace SeedPoolServicer
{

// 定义 FileSeedPoolThread 种子池类
class FileSeedPoolThread : public SeedPoolThread
{
public:
    using SharedPtr = std::shared_ptr<FileSeedPoolThread>;
public:
    // a seed_pool_thread with init_seed_folder
    FileSeedPoolThread(
        const std::string & seed_pool_name,     // 种子池名称
        bool ave_possibility = true           // 是否平均概率
    ); 

    // APIs
    void init_seeds(const std::string & directoryPath) override;  // 初始种子池
    // void launch();
    // void shutdown();
    void restore_for_future(int num=-1);    // 保存种子池中最有价值的num个种子（保存到初始种子池中）, -1表示全部保存
    void clean_up_tmp_seeds();              // 种子池清理（删除tmp_seed_dir目录文件）

    ~FileSeedPoolThread(){};

protected:
    // copy seed_str(file) into tmp_seed_pool
    void StoreSeedRequestCallback(const std::string & file_path) override;

protected:
    // tmp_seed_folder_ is created for recorded seed-files
    std::filesystem::path tmp_seed_folder_;
    std::filesystem::path name_seed_folder_;
    std::string init_seed_folder_{""};
    void restore_for_future_all();


protected:
    // readtime
    std::string stamp()
    {
        // 获取当前时间
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm * localTime = std::localtime(&currentTime);
        // 格式化时间为字符串
        std::stringstream ss;
        ss << std::put_time(localTime, "%Y-%m-%d-%H-%M-%S");
        return ss.str();
    }
};


}

#endif
