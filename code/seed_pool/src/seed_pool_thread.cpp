#include <iostream>
#include "seed_pool_thread.hpp"

namespace SeedPoolServicer
{

SeedPoolThread::SeedPoolThread(const std::string & seed_pool_name)
: SeedPoolThread(seed_pool_name, true) {}

SeedPoolThread::SeedPoolThread(const std::string & seed_pool_name, const bool & ave_possibility)
:   SeedPool(ave_possibility),
    seed_pool_name_(seed_pool_name),
    pipe_name_PoolToWork_("/tmp/" + seed_pool_name + "_PoolToWork"),  // seed pool 从这里发
    pipe_name_WorkToPool_("/tmp/" + seed_pool_name + "_WorkToPool"),   // seed pool 从这里读
    pipe_fd_PoolToWork_(-1),    // Initialize to invalid values
    pipe_fd_WorkToPool_(-1)      // Initialize to invalid values
{
}

void
SeedPoolThread::launch()
{
    // 创建管道  
    mkfifo(pipe_name_PoolToWork_.c_str(), 0666);
    mkfifo(pipe_name_WorkToPool_.c_str(), 0666);
    std::cout  << "[seed_pool::" << seed_pool_name_ << "]:"<< " pipes created (inactivated)." << std::endl;
    // activate  // 启动SeedPool分支thread，thread生命周期交给exit_flag_信号处理。
    state_ = true;
    callback_thread_ = std::thread(&SeedPoolThread::startCallbackThread, this);
    update_thread_ = std::thread(&SeedPoolThread::startUpdateThread, this);
    std::cout << "[seed_pool::" << seed_pool_name_ << "]:" << " Launched (activated). " << std::endl;
}

void
SeedPoolThread::shutdown()
{
    // deactivate
    exit_flag_ = true;
    callback_thread_.join(); // wait for exit;
    update_thread_.join(); // wait for exit;
    std::cout << "[seed_pool::" << seed_pool_name_ << "]:" 
        << " shutdown" << std::endl;
    // clean
    closePipes();  // 关闭管道
    delPipes();  // 删除管道
    std::cout << "[seed_pool::" << seed_pool_name_ << "]:" 
        << " clean up. pipes destroyed." << std::endl;
    return ; 
}

// -------------------threads----------------------------- //
void SeedPoolThread::startCallbackThread()
{
    // 回调函数 工作机制
    while (!exit_flag_) {
        std::string message = "";
        if (openPipes()) {
        // 管道读取
            message = ReadPipeFd(pipe_fd_WorkToPool_, 2048);
            // 缓冲区大小 为100，因为请求指令就2种
        }
        // message 为获取的信息
        // 触发回调
        if (!message.empty()) {
            // 读取到信息
            if (message == PICK_SEED_REQUEST) {
                // std::cout << "[debug][seed_pool_thread]: recieve a pick request from user." << std::endl;
                // pick种子请求
                bool success = false;
                while(!success && !exit_flag_)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));      // 10 毫秒判断一次
                    success = PickSeedRequestCallback();
                    if(!success) std::cerr << "waiting for prepared seeds...." << std::endl;
                }
            }
            else {
                // store种子请求
                StoreSeedRequestCallback(message);
            }
        }
        // 控制读取频率
        std::this_thread::sleep_for(std::chrono::milliseconds(100));      // 100 毫秒判断一次
    }
    /* @brief 关闭种子池
    * 工作进程 向pipe发布： SHUTDOWN_REQUEST
    * 触发种子池 回调函数： 退出种子池进程
    */
    state_ = false;
}
void
SeedPoolThread::startUpdateThread()
{
    // store new seed and update prepared_seed
    while (!exit_flag_) {
        update_stored_seeds();
        update_prepared_seeds();
        
        // 控制读取频率
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100 毫秒判断一次
    }
    return;
}

// ----------------------socket callbacks ---------------------------/

bool SeedPoolThread::PickSeedRequestCallback()
{
    /* @brief pick一个seed
    * 协议流程：
    *   工作进程 向pipe发布： PICK_SEED_REQUEST
    *   触发种子池 回调函数： 回调函数向pipe发布 一个seed
    *   本次通信结束。
    */
    std::string picked_seed = pop_prepared_seed(); // 优先队列操作保护
    if(picked_seed.empty()) return false;
    // std::cout << "[debug][seed_pool_thread]: I pick the data " << picked_seed << std::endl;

    // send to socket
    send_SEED(picked_seed);
    // std::cout << "[debug][seed_pool_thread]: I send the picked data to user" << std::endl;
    return true;
}

void SeedPoolThread::StoreSeedRequestCallback(const std::string & seed_str)
{
    /* @brief 保存一个seed
    * 协议流程：
    *   工作进程 向pipe发布： STORE_SEED_REQUEST
    *   触发种子池 回调函数： 回调函数向pipe发布一个I_M_HERE，并等待管道接下来的正文
    *   工作进程 读取到pipe中的 I_M_HERE后，向pipe发布 一个字符串，
    *   这个字符串由Seed类解析而成,包含种子内容和权重
    *   种子池 读取并进行store操作
    *   本次通信结束。
    */

    /* @brief 范例函数：StoreSeedRequestCallback
    *  override的时候，务必按照以下【】要求进行改写，
    *  且要保证每个【】模块的顺序与此模板一致
    */
    // 【可选项】 对种子文件进行转存、处理等行为

    // 【必写项】 存储 & 反馈I'm here信息
    unstored_seed_.push(Seed::parseFromString(seed_str));
    send_I_M_HERE();
    // std::cout << "Servicer DEBUG: I stored the seed" << std::endl;
    return;
}

// ----------------------- update callbacks ------------------------------/
void SeedPoolThread::update_prepared_seeds()
{
    /* @brief 范例函数：update_prepared_seeds
    *  override的时候，务必按照以下【】要求进行改写，
    *  且要保证每个【】模块的顺序与此模板一致
    */
    // 【可改项】 防止prepared_seed过多膨胀，这里可以添加预备种子上限个数检查，比如：
    if(prepared_seeds_.size()>=5) return ; 
    // 【必写项】 pick种子
    std::string seed_data = pickSeed().seedData;
    // 【可选项】 mutator逻辑：
        // seedData = mutator.mutate(seedData);
    // 【必写项】 
    push_prepared_seed(seed_data);
    // std::cout << "[debug][seed_pool_thread]: update prepared_seed." << std::endl;
    return ; 
}

void SeedPoolThread::update_stored_seeds()
{
    if (!unstored_seed_.empty()) {
        addDataInSeedPool(unstored_seed_.front());
        unstored_seed_.pop();
    }
}

// ------------------------Pipes Operations:-----------------------------------//

void SeedPoolThread::createPipes()
{
    // 创建管道
    mkfifo(pipe_name_PoolToWork_.c_str(), 0666);
    mkfifo(pipe_name_WorkToPool_.c_str(), 0666);
    return ; 
}

bool SeedPoolThread::openPipes()
{
    if (pipe_fd_PoolToWork_ != -1 && pipe_fd_WorkToPool_ != -1) {
        return true;
    }
    // 打开管道
    // pipe_fd_PoolToWork_ = open(pipe_name_PoolToWork_.c_str(), O_WRONLY);   // pool 只写
    // pipe_fd_WorkToPool_ = open(pipe_name_WorkToPool_.c_str(), O_RDONLY);   // pool 只读
    pipe_fd_PoolToWork_ = open(pipe_name_PoolToWork_.c_str(), O_WRONLY | O_NONBLOCK | O_CLOEXEC); 
            // pool 只写  // 非阻塞打开  // 文件描述符不会被fork进程继承
    pipe_fd_WorkToPool_ = open(pipe_name_WorkToPool_.c_str(), O_RDONLY | O_NONBLOCK | O_CLOEXEC); 
            // pool 只读  // 非阻塞打开  // 文件描述符不会被fork进程继承
    
    //  这里我们使用非阻塞，从而保证线程可以退出
    /* @brief mkfifo知识
        * 在不进行特殊声明的情况下，open会导致进程阻塞，直到有其他进程也open了该管道。
        *
        * 如果open调用是阻塞的:
        * 即: 如果以只读形式打开管道，则等到其他进程以写的方式打开管道，才会继续进程；
        *     如果以只写形式打开管道，则等到其他进程以读的方式打开管道，才会继续进程。
        * 好处是：如果是双方通信，保证了后续操作 在配对后 再执行。
        *
        * 如果open调用是非阻塞的:
        * open总会立即返回，但如果没有其他进程以只读方式打开同一个FIFO文件，open调用将返回-1，并且FIFO也不会被打开.
        * 适合open和read连用，适用于即时通信(用户端)
        *
        * reference: https://www.cnblogs.com/52php/p/5840229.html
        * Addition: multi-writer and multi-reader situation need more design limits,
        * fortunately,for the seed pool,
        * we don't need to consider the situation that multi-writer within one mkfifo pipe.
        */
    if (pipe_fd_PoolToWork_ == -1 || pipe_fd_WorkToPool_ == -1) {
        // have not been matched
        // 和对方管道匹配失败，返回失败
        return false;
    } else {
        // std::cout<<"DEBUG:  "<<pipe_fd_PoolToWork_ << " "<< pipe_fd_WorkToPool_<<std::endl;
    }
    return true;
}

void SeedPoolThread::closePipes()
{
    // 关闭管道
    if (pipe_fd_PoolToWork_ != -1) {
        close(pipe_fd_PoolToWork_);
    }
    if (pipe_fd_WorkToPool_ != -1) {
        close(pipe_fd_WorkToPool_);
    }
}

void SeedPoolThread::delPipes()
{
    // 删除管道
    unlink(pipe_name_PoolToWork_.c_str());
    unlink(pipe_name_WorkToPool_.c_str());
}


std::string SeedPoolThread::ReadPipeFd(const int & pipe_fd, const int & buffer_SIZE)
{
    char buffer[buffer_SIZE];
    ssize_t totalBytesRead = 0;
    while (!exit_flag_) {// 读取（防止读到半条信息的情况
        ssize_t bytesRead = read(pipe_fd, buffer + totalBytesRead, buffer_SIZE - totalBytesRead - 1);
        if (bytesRead == -1) {
        break;  // 如果read返回-1，表示没有更多数据可读
        } else if (bytesRead == 0) {   // 读取完毕
        break;      // 如果read返回0，表示没有更多数据可读
        }
        totalBytesRead += bytesRead;
        if (totalBytesRead >= buffer_SIZE - 1) {   // 防止缓冲区溢出
        perror("wrong order in communication with SeedPoolThread");
        exit(EXIT_FAILURE);
        }
    }
    buffer[totalBytesRead] = '\0';   // 手动添加 null 终止符
    std::string message(buffer, totalBytesRead);
    // std::cout << "Servicer "<<seed_pool_name_<< " DEBUG:  I read the msg:" << message << std::endl;
    return message;
}


}
