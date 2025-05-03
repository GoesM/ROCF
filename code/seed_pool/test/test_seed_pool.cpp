#include "seed_pool.hpp"
#include <filesystem>
namespace fs = std::filesystem;

// 获取当前程序所在的目录
fs::path currentPath = fs::current_path();
// 构建上级目录的路径
fs::path parentPath = currentPath.parent_path();
// 构建目标文件夹的路径
fs::path targetFolderPath = parentPath / "../test/input_seed";


int main()
{
  SeedPool seedPool(false);
  if (!seedPool.empty()) {
    perror("function empty error");
    exit(EXIT_FAILURE);
  }
  Seed A = {3, "SeedA"};
  std::string A_str = A.toString();
  Seed B = Seed::parseFromString(A_str);
  B.seedData += "BBB";
  Seed C = {1, "SeedC"};
  // 添加一些带权重的种子到种子池中
  seedPool.addDataInSeedPool(A);
  seedPool.addDataInSeedPool(B);
  seedPool.addDataInSeedPool(C);

  seedPool.init_seeds(targetFolderPath);

  while (true) {
    int a;std::cin >> a;
    // 从种子池中随机选取种子
    Seed selectedSeed = seedPool.pickSeed();

    // 输出选取的种子
    std::cout << "Selected seed: " << selectedSeed.seedData << std::endl;
  }

  return 0;
}
