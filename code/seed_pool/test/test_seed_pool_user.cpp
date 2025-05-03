#include <iostream>
#include <thread>
#include "seed_pool_user.hpp"

int main()
{
  std::cout << "---------seed_pool_manager test.---------" << std::endl;
  
  // indirect API
  SeedPoolUser::SeedPoolUser seed_pool_user("seed_pool_1");  // 种子池名称
  std::string picked_seed = seed_pool_user.pickSeed();  // 种子pick： pick一个种子
  std::cout << "pick seed:" << picked_seed << std::endl;
  seed_pool_user.addSeed(picked_seed, 1);  // 种子储存： 种子数据为"Seed_1"， 种子权重为 1  
  
  // direct API
  SeedPoolUser::addSeedInPool("seed_pool_2", picked_seed,4);
  picked_seed = SeedPoolUser::pickSeedFromPool("seed_pool_2");
  std::cout << "pick seed:" << picked_seed << std::endl;

  return 0;
}
