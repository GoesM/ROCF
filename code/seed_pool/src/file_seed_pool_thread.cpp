#include "file_seed_pool_thread.hpp"
// record file-path
// all seed context is in file

namespace SeedPoolServicer
{

FileSeedPoolThread::FileSeedPoolThread(
    const std::string & seed_pool_name,
    bool ave_possibility)
: SeedPoolThread(seed_pool_name, ave_possibility)
{
    // create tmp_seed_folder and create
    std::filesystem::path currentPath = std::filesystem::current_path();

    tmp_seed_folder_ = currentPath / "tmp_seed";
    name_seed_folder_ = tmp_seed_folder_ / (seed_pool_name + "_tmp_seeds");

    std::filesystem::create_directories(name_seed_folder_);
}

// -------------------新API--------------------- // 

// just copy these files into tmp_seed_folder
void FileSeedPoolThread::init_seeds(const std::string & directoryPath)
{
    if (init_seed_folder_.empty()) {init_seed_folder_ = directoryPath;}

    // 遍历源文件夹中的所有文件
    for (const auto & entry : std::filesystem::directory_iterator(directoryPath)) {
        if (std::filesystem::is_directory(entry.path())) {
            // 如果是文件夹，step into
            init_seeds(entry.path().string());
            continue;
        }
        addDataInSeedPool( (Seed) {1, entry.path().string()});
    }
}


// 将临时种子文件保存到init_seed_pool中
void FileSeedPoolThread::restore_for_future(int num)
{
  if (init_seed_folder_.empty()) {
    std::cerr << "[FileSeedPoolThread]: set up init_seed_folder_ before restore_for_future(), please" << std::endl;
    std::cerr << "[FileSeedPoolThread]: seeds are aborted" << std::endl;
    return;
  }
  if(num==-1){
    restore_for_future_all();
    return ; 
  }
  if(num<=0){
    std::cerr << "[FileSeedPoolThread::restore_for_future]: you set num as a negative number:"<< num << std::endl;
    return ; 
  }

  std::filesystem::path targetDirPath = std::filesystem::path(init_seed_folder_) /
    (stamp() + seed_pool_name_);
  std::filesystem::create_directory(targetDirPath);
  // 循环遍历前 num 个seed
  for (int i = 0; i < num && i < mirror_seed_pool_.size(); ++i)
  {
    const auto & seed = mirror_seed_pool_[i];
    std::filesystem::path filePath = seed.seedData;

    std::filesystem::path targetPath = targetDirPath /
    (seed_pool_name_ + stamp() + filePath.filename().string());

    std::filesystem::copy(filePath, targetPath);
  }

  return ;
}

void FileSeedPoolThread::restore_for_future_all()
{// default: = init_seed_folder_
  std::filesystem::path targetPath = std::filesystem::path(init_seed_folder_) /
    (stamp() + seed_pool_name_);
  std::filesystem::rename(name_seed_folder_, targetPath);
  return;
}

// @todo clean-up tmp_seed_folder
void FileSeedPoolThread::clean_up_tmp_seeds()
{
  std::filesystem::remove_all(name_seed_folder_);
  return;
}


// @todo copy seed_str ( Seed{value,filePath} ) into tmp_seed_pool
//          and the recorded seed file-path is in tmp_seed_pool folder
void FileSeedPoolThread::StoreSeedRequestCallback(const std::string & seed_str)
{
  Seed ls_seed = Seed::parseFromString(seed_str);
  const std::string file_path = ls_seed.seedData;
  
  // copy it into name_seed_folder
  std::filesystem::path sourcePath(file_path);
  std::filesystem::path targetPath = name_seed_folder_ / ("seed_" + stamp());
  std::filesystem::copy(sourcePath, targetPath, std::filesystem::copy_options::overwrite_existing);
  ls_seed.seedData = targetPath;

  // 【必写项】 存储 & 反馈I'm here信息
  unstored_seed_.push(ls_seed);
  send_I_M_HERE();
  // std::cout << "Servicer DEBUG: I stored the seed" << std::endl;
  return;
}


}
