#ifndef SEED_POOL_THREAD_HPP
#define SEED_POOL_THREAD_HPP

#include "seed_pool.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <chrono>
#include <sys/stat.h>
#include <cstdlib>
#include <sys/types.h>
#include <mutex>


/* @brief 通信格式
 * pick一个seed:
   * 工作进程 向pipe发布： PICK_SEED_REQUEST
   * 触发种子池 回调函数： 回调函数向pipe发布 一个seed
   * 本次通信结束。
 * store一个seed
   * 工作进程 向pipe发布： STORE_SEED_REQUEST
   * 触发种子池 回调函数： 回调函数向pipe发布一个I_M_HERE，并等待管道接下来的正文
   * 工作进程 读取到pipe中的 I_M_HERE后，向pipe发布 一个字符串，表示种子内容
   * 种子池 读取并反馈:查询种子权重请求
   * 工作进程读取请求后，传输权重
   * 种子池读取权重后，进行store操作
   * 本次通信结束。
 */
namespace SeedPoolServicer
{

class SeedPoolThread : public SeedPool
{
public:
    using SharedPtr = std::shared_ptr<SeedPoolThread>;
public:  // API for usage.
    SeedPoolThread(const std::string & seed_pool_name);
    SeedPoolThread(const std::string & seed_pool_name, const bool & ave_possibility);

    void launch();
    void shutdown();

    ~SeedPoolThread(){};

protected:
  std::string seed_pool_name_;   // 必须是一个特殊的命名，这涉及管道名称的声用

private: // pipes
    std::string pipe_name_PoolToWork_;
    std::string pipe_name_WorkToPool_;
    int pipe_fd_PoolToWork_;
    int pipe_fd_WorkToPool_;

    void createPipes();
    bool openPipes();
    void closePipes();
    void delPipes();


private: 
    /* @brief 双线程管理策略： 更新线程 & 回调响应线程
    * 种子池会维护一个unstored_seed_queue_序列，提升store速度
    * callback对store的响应，only存放在预存放序列unstored_seed_
    * update_thread监控unstored_seed_不为空，则进行维护操作
    */
    
    // the thread for updating seed_pool
    std::thread update_thread_; 
    void startUpdateThread(); // 不断更新种子池内部数据，update_seed_pool

    // the thread for callback function
    std::thread callback_thread_; 
    void startCallbackThread();  // 识别user通信，并调用对应callback

    // lifecycle
    bool state_{false}; // if being launched
    bool exit_flag_{false}; // if user request exit

protected: // callback 函数， 用于响应
    bool PickSeedRequestCallback();
    virtual void StoreSeedRequestCallback(const std::string & seed_str);

    /* @brief 预备种子机制
    * 种子池会维护 一些 预备种子，以节省pick时间。
    * 当获得pick请求时，将预备种子放入管道中，并重置预备种子
    * update_prepared_seeds() is used in "startUpdateThread()"
    */
    std::queue<std::string> prepared_seeds_;      // 预备种子
    virtual void update_prepared_seeds();         // update预备种子，可以通过override来添加mutator机制等。
    std::mutex queue_lock_;
    std::string pop_prepared_seed()  // pick
    {
        queue_lock_.lock();  // 优先队列操作保护
        if(prepared_seeds_.empty()) return "";
        std::string picked_seed = prepared_seeds_.front();
        prepared_seeds_.pop();
        queue_lock_.unlock(); // 提前解锁，保证效率
        return picked_seed;
    }
    void push_prepared_seed(const std::string & seed_data)
    {
        queue_lock_.lock();  // 优先队列操作保护
        prepared_seeds_.push( seed_data );   // pick for preparation.
        queue_lock_.unlock(); // 提前解锁，保证效率
        return ;
    }

    /* @brief 预存种子机制
    *  种子池收到一个种子后，在callback函数中，不会直接把它塞入优先队列（队列维护O(n)）
    *  而是先放在“预存种子”里，等待处理，在update_thread_中进行处理
    */
    std::queue<Seed> unstored_seed_;
    void update_stored_seeds();

protected:
  inline void send_I_M_HERE()
    {write(pipe_fd_PoolToWork_, I_M_HERE.c_str(), I_M_HERE.size());}
  inline void send_SEED(const std::string & picked_seed)
    {write(pipe_fd_PoolToWork_, picked_seed.c_str(), picked_seed.size());}

private:
  // 定义通信的请求类型
  const std::string PICK_SEED_REQUEST = "randomly pick a seed from pool";  // 请求pick一个种子
  const std::string STORE_SEED_REQUEST = "store a seed into pool";         // 请求保存种子
  const std::string SHUTDOWN_REQUEST = "shutdown the SeedPoolProcess";    // 请求关闭种子池进程
  // 定义通信的反馈类型
  const std::string I_M_HERE = "I'm here";   // 表示响应
  const std::string WHAT_VALUE = "what's the value?"; // request the value of seed

  // read pipe
  std::string ReadPipeFd(const int & pipe_fd, const int & buffer_SIZE);
};

}
#endif
