#include <iostream>
#include <unistd.h>
#include <string>
#include <random>
#include <vector>
#include <queue>
#include <algorithm>

#include "seed_pool.hpp"

/*#----------------------------------seed pool------------------------------#*/
// 定义 seedPool 种子池类
SeedPool::SeedPool(const bool & ave_possibility)
{
  ave_possibility_ = ave_possibility;
}
SeedPool::~SeedPool() {}

Seed
SeedPool::pickSeed() const
{
  if (!seed_pool_.empty()) {
    return pickRandomSeed();     // 从当前优先队列中返回一个随机种子
  } else {
    return Seed{0, ""};          // 返回一个默认值  none seed
  }
}


void 
SeedPool::show_seed_pool()
{
  // queue
  std::cout << "in queue seed_pool:" << std::endl;
  std::priority_queue<Seed> tempQueue = seed_pool_;
  while (!tempQueue.empty()) {
    Seed tmp = tempQueue.top();
    std::cout << "seed: " << tmp.seedData << "; value:= " << tmp.value << std::endl;
    tempQueue.pop();
  }
  
  // mirror
  std::cout << "in mirror seed_pool:" << std::endl;
  for (const auto & seed : mirror_seed_pool_) {
    std::cout << "seed: " << seed.seedData << "; value:= " << seed.value << std::endl;
  }
  return ; 
}

void
SeedPool::addDataInSeedPool(const Seed seed_)
{
  seed_pool_.push(seed_);
  // 更新 到 vector镜像中
  std::priority_queue<Seed> tempQueue = seed_pool_;
  /* @brief 快速pick，因此镜像操作放在add
       * 基于分支进程管理seedPool的使用逻辑，对add操作的速度要求不高，但对pick的速度要求很高
       * 每次输出都需要pick，且pick全部完成后才能执行fuzz功能，因此对pick的速度要求很高
       * add操作次数较少，且add之后无需等待add操作完全完成就可以执行后续操作
       */
  mirror_seed_pool_.clear();  // addData & pickseed is in one thread(callback), so there's no worry about data-race
  while (!tempQueue.empty()) {
    mirror_seed_pool_.push_back(tempQueue.top());
    tempQueue.pop();
  }

  // show_seed_pool();
  return;
}

bool
SeedPool::empty()
{
  return seed_pool_.empty();
}
void
SeedPool::init_seeds(const std::string & directoryPath)
{
  readFilesInDirectory(directoryPath);
  return;
}


Seed
SeedPool::ave_pickSeed() const
{
  // 随机索引
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, mirror_seed_pool_.size() - 1);
  // 返回
  return mirror_seed_pool_[dis(gen)];
}

Seed
SeedPool::val_pickSeed() const
{
  // [TODO] 实现按权重随机的最简单方法是线性扫描，有没有更好的方法？

  // 计算总权重
  int totalWeight = 0;
  for (const auto & seed : mirror_seed_pool_) {
    totalWeight += seed.value;
  }

  // 随机生成一个权重值
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, totalWeight);
  int randomWeight = dis(gen);

  // 根据权重值选择种子
  for (const auto & seed : mirror_seed_pool_) {
    randomWeight -= seed.value;
    if (randomWeight <= 0) {
      return seed;
    }
  }
  return mirror_seed_pool_[mirror_seed_pool_.size() - 1];
}

Seed
SeedPool::pickRandomSeed() const
{
  if (ave_possibility_) {return ave_pickSeed();} // 同概率随机选取
  else                  {return val_pickSeed();} // 权重概率随机选取
}

void
SeedPool::readSeedFile(const std::string & filePath)
{
  std::wifstream file(filePath);
  if (!file.is_open()) {
    perror("no context in your seed file!");
    exit(EXIT_FAILURE);
  }

  // 设置locale以支持UTF-8编码
  std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
  file.imbue(utf8_locale);
  std::wstringstream contentStream;
  contentStream << file.rdbuf();
  file.close();

  // 读取的内容转为UTF-8编码的字符串
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::string utf8Content = converter.to_bytes(contentStream.str());
  // std::cout << utf8Content << std::endl;
  addDataInSeedPool( (Seed) {1, utf8Content});
}

void
SeedPool::readFilesInDirectory(const std::string & directoryPath)
{
  try {
    for (const auto & entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
      if (std::filesystem::is_regular_file(entry.path())) {
        // 处理文件，这里可以插入你的文件读取逻辑
        readSeedFile(entry.path().c_str());
        // std::cout << "Reading file: " << entry.path() << std::endl;
      }
    }
  } catch (const std::exception & e) {
    perror("error in reading files");
    exit(EXIT_FAILURE);
  }
}
