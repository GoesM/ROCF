#ifndef SEED_POOL_HPP
#define SEED_POOL_HPP

#include <string>
#include <vector>
#include <queue>

#include <iostream>
#include <sstream>
#include <fstream>


#include <filesystem>
// namespace fs = std::filesystem;

// 定义 Seed 结构体
struct Seed
{
  int value = 0;           // 优先级权重
  std::string seedData = ""; // 数据内容
  /* @brief notice
   * space is allowed in seedData,
   * '\n' is not allowed
   */

  // 构造函数，接受花括号初始化列表
  Seed(int v, const std::string & data)
  : value(v), seedData(data) {}
  // 重载小于运算符
  bool operator<(const Seed & other) const
  {
    return value < other.value;     // 大顺序，越大越靠前
  }
  //
  // // 重载小于运算符
  // bool operator>(const Seed & other) const
  // {
  //   return value > other.value;     // 大顺序，越大越靠前
  // }

  // 解析字符串到 Seed 结构体
  static Seed parseFromString(const std::string & str)
  {
    std::istringstream iss(str);
    int value;
    std::string seedData;

    if (iss >> value) {
      // Read the entire line after the value as seedData
      std::getline(iss >> std::ws, seedData);

      return Seed(value, seedData);
    } else {
      // 处理解析失败的情况
      // 可以抛出异常或者返回一个默认值，这里返回一个默认值
      perror("wrong string");
    }

    return Seed(0, "");
  }

  // 将 Seed 结构体转为字符串
  std::string toString() const
  {
    std::ostringstream oss;
    oss << value << " " << seedData;
    return oss.str();
  }
};


// 定义 seedPool 种子池类
class SeedPool
{
public:
  SeedPool();
  SeedPool(const bool & pick_mode);
  ~SeedPool();

  Seed pickSeed() const;
  void addDataInSeedPool(const Seed seed_);
  bool empty();
  virtual void init_seeds(const std::string & directoryPath);

  void show_seed_pool();

protected:
  std::priority_queue<Seed> seed_pool_;    // 将种子以seed类型存入seed_pool_ (value优先级权重，value越大越优先，)
  std::vector<Seed> mirror_seed_pool_;   // vector镜像，方便pick
  bool ave_possibility_{true};   // 是否平均概率

  Seed ave_pickSeed() const;
  Seed val_pickSeed() const;
  Seed pickRandomSeed() const;

  void readSeedFile(const std::string & filePath);
  void readFilesInDirectory(const std::string & directoryPath);
};


#endif
