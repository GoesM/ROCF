#ifndef SEED_POOL_USER_HPP
#define SEED_POOL_USER_HPP

#include "seed_pool.hpp"
/* @brief 通信格式
 * pick一个seed:
    工作进程 向pipe发布： PICK_SEED_REQUEST
    触发种子池 回调函数： 回调函数向pipe发布 一个seed
    本次通信结束。
 * store一个seed
    工作进程 向pipe发布： STORE_SEED_REQUEST
    触发种子池 回调函数： 回调函数向pipe发布一个I_M_HERE，并等待管道接下来的正文
    工作进程 读取到pipe中的 I_M_HERE后，向pipe发布 一个字符串
    种子池 读取并保存
 */

namespace SeedPoolUser
{
// direct API
void addSeedInPool(
  const std::string & seed_pool_name, // 种子池名称
  const std::string & seed_context,   // 种子内容
  const int & val                     // 种子权重
);
std::string pickSeedFromPool(const std::string & seed_pool_name);


// indirect API
class SeedPoolUser
{
public:
  SeedPoolUser(const std::string & seed_pool_name);
  void addSeed(const std::string & seed_context, const int & val); // 种子内容，种子权重
  std::string pickSeed();
  ~SeedPoolUser(){};

private:
  std::string seed_pool_name_;   // 必须是一个特殊的命名，这涉及管道名称的声用
  std::string pipe_name_PoolToWork_;
  std::string pipe_name_WorkToPool_;
  int pipe_fd_PoolToWork_;
  int pipe_fd_WorkToPool_;

  bool openPipes();
  void closePipes();

  std::string PickSeedRequest();
  void StoreSeedRequest(const Seed & seed_);

  // 定义通信的请求类型
  const std::string PICK_SEED_REQUEST = "randomly pick a seed from pool";  // 请求pick一个种子
  const std::string STORE_SEED_REQUEST = "store a seed into pool";
  // 定义通信的反馈类型
  const std::string I_M_HERE = "I'm here";   // 表示响应

  std::string ReadPipeFd(const int & pipe_fd, const int & buffer_SIZE);
};


}


#endif
