#include <iostream>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>


#include <string>
#include <fstream>
#include <thread>
#include "seed_pool.hpp"
/* @brief use following code:
 * struct Seed{int val,std::string seedData}
 */
#include "seed_pool_user.hpp"


namespace SeedPoolUser
{

// direct API
void addSeedInPool(
  const std::string & seed_pool_name, const std::string & seed_context,
  const int & val)
{
  SeedPoolUser adder(seed_pool_name);
  adder.addSeed(seed_context, val);
}

std::string pickSeedFromPool(const std::string & seed_pool_name)
{
  SeedPoolUser picker(seed_pool_name);
  return picker.pickSeed();
}



SeedPoolUser::SeedPoolUser(const std::string & seed_pool_name)
:  seed_pool_name_(seed_pool_name),
  pipe_name_PoolToWork_("/tmp/" + seed_pool_name + "_PoolToWork"),  // seed pool 从这里发
  pipe_name_WorkToPool_("/tmp/" + seed_pool_name + "_WorkToPool"),   // seed pool 从这里读
  pipe_fd_PoolToWork_(-1),     // Initialize to invalid values
  pipe_fd_WorkToPool_(-1)      // Initialize to invalid values
{}

void
SeedPoolUser::addSeed(const std::string & seed_context, const int & val)
{
  if (!openPipes()) {
    perror("please create SeedPool firstly, within the same name");
    exit(EXIT_FAILURE);
  }
  StoreSeedRequest(Seed{val, seed_context});
  closePipes();
  return;
}

std::string SeedPoolUser::pickSeed()
{
  if (!openPipes()) {
    perror("please create SeedPool firstly, within the same name");
    exit(EXIT_FAILURE);
  }
  std::string picked_seed_str =  PickSeedRequest();
  closePipes();
  return picked_seed_str;
}


void SeedPoolUser::StoreSeedRequest(const Seed & seed_)
{
  /* @brief 保存一个seed
   * 工作进程 向pipe发布： STORE_SEED_REQUEST
   * 触发种子池 回调函数： 回调函数向pipe发布一个I_M_HERE，并等待管道接下来的正文
   * 工作进程 读取到pipe中的 I_M_HERE后，向pipe发布 一个字符串，
   * 这个字符串由Seed类解析而成,包含种子内容和权重
   * 种子池 读取并进行store操作
   * 本次通信结束。
   */

  // 发送一个种子内容
  std::string seed_str = seed_.toString();
  write(pipe_fd_WorkToPool_, seed_str.c_str(), seed_str.size());
  std::string callbackMSG = "";
  while (callbackMSG.empty()) {
    // std::cout<<"User Debug: i'm waiting for callback\n";
    callbackMSG = ReadPipeFd(pipe_fd_PoolToWork_, 100);
  }
  if (callbackMSG != I_M_HERE) {
    perror("I_M_HERE signal not catched");
    exit(EXIT_FAILURE);
  }
  // std::cout<<"User Debug: i catch the callback\n";
  return;
}

std::string
SeedPoolUser::PickSeedRequest()
{
  /* @brief pick一个seed
   * 工作进程 向pipe发布： PICK_SEED_REQUEST
   * 触发种子池 回调函数： 回调函数向pipe发布 一个seed
   * 本次通信结束。
   */

  // 发送一个pick种子请求
  write(pipe_fd_WorkToPool_, PICK_SEED_REQUEST.c_str(), PICK_SEED_REQUEST.size());
  std::string callbackSeed = "";

  while (callbackSeed.empty()) {
    callbackSeed = ReadPipeFd(pipe_fd_PoolToWork_, 1024);
  }

  // return the content of seed
  return callbackSeed;
}


bool
SeedPoolUser::openPipes()
{
  if (pipe_fd_PoolToWork_ != -1 && pipe_fd_WorkToPool_ != -1) {
    return true;
  }
  // 打开管道
  pipe_fd_PoolToWork_ = open(pipe_name_PoolToWork_.c_str(), O_RDONLY | O_CLOEXEC); 
      // user 只读, 阻塞匹配  // 不被fork进程复制资源
  pipe_fd_WorkToPool_ = open(pipe_name_WorkToPool_.c_str(), O_WRONLY | O_CLOEXEC);   
      // user 只写, 阻塞匹配  // 不被fork进程复制资源
  // pipe_fd_PoolToWork_ = open(pipe_name_PoolToWork_.c_str(), O_WRONLY | O_NONBLOCK); // non block open
  // pipe_fd_WorkToPool_ = open(pipe_name_WorkToPool_.c_str(), O_RDONLY | O_NONBLOCK); // non block open
  //  这里我们使用阻塞，从而保证可以与线程建立通信
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
    return false;
  } else {
    // std::cout<<"DEBUG:  "<<pipe_fd_PoolToWork_ << " "<< pipe_fd_WorkToPool_<<std::endl;
    // std::cout << "DEBUG: User open pipes:" << std::endl;
    // std::cout << pipe_name_PoolToWork_ << " : msg from PoolProcess to WorkProcess" << std::endl;
    // std::cout << pipe_name_WorkToPool_ << " : msg from WorkProcess to PoolProcess" << std::endl;
  }
  return true;
}

std::string
SeedPoolUser::ReadPipeFd(const int & pipe_fd, const int & buffer_SIZE)
{
  char buffer[buffer_SIZE];
  ssize_t totalBytesRead = 0;
  while (true) {// 读取（防止读到半条信息的情况
    ssize_t bytesRead = read(pipe_fd, buffer + totalBytesRead, buffer_SIZE - totalBytesRead - 1);
    if (bytesRead == -1) {
      break;  // 如果read返回-1，表示没有更多数据可读
    } else if (bytesRead == 0) {   // 读取完毕
      break;      // 如果read返回0，表示没有更多数据可读
    }
    totalBytesRead += bytesRead;
    if (totalBytesRead >= buffer_SIZE - 1) {   // 防止缓冲区溢出
      perror("wrong order in communication with SeedPoolUser");
      exit(EXIT_FAILURE);
    }
    break; // 不知道为啥,反正加上就对了
  }
  buffer[totalBytesRead] = '\0';   // 手动添加 null 终止符
  std::string message(buffer, totalBytesRead);
  // std::cout << "User DEBUG:  I read the msg:" << message << std::endl;
  return message;
}

void
SeedPoolUser::closePipes()
{
  // 关闭管道
  if (pipe_fd_PoolToWork_ != -1) {
    close(pipe_fd_PoolToWork_);
    pipe_fd_PoolToWork_ = -1; 
  }
  if (pipe_fd_WorkToPool_ != -1) {
    close(pipe_fd_WorkToPool_);
    pipe_fd_WorkToPool_ = -1;
  }
  return;
}


}
