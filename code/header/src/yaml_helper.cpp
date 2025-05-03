#include "yaml_helper.hpp"



YAML::Node
yaml_helper::yaml_value()
{
    return yaml_node_context_;
}

std::string
yaml_helper::yaml_value_str()
{
    return yaml_str_context_;
}



void 
yaml_helper::write_into_file(const std::string & log_file_path_)
{
    if (!yaml_file_path_.empty()) {
        try {
        // 源文件路径
        std::filesystem::path sourcePath(yaml_file_path_);
        // 目标文件路径
        std::filesystem::path destinationPath(log_file_path_);
        // 复制文件
        std::filesystem::copy(
            sourcePath, destinationPath,
            std::filesystem::copy_options::overwrite_existing);          // overwrite

        // std::cout << "File copied successfully." << std::endl;
        } catch (const std::exception & e) {
        std::cerr << "Error: " << e.what() << std::endl;
        }
    } else if (yaml_node_context_.IsDefined()) {
        yaml_str_context_ = YAML::Dump(yaml_node_context_);
        // 打开文件流以写入YAML文件
        std::ofstream fout(log_file_path_);
        // 写入文件流
        fout << yaml_str_context_;
        // 关闭文件流
        fout.close();
    } else {
        perror("you've not get yamlfile or yamlcontext first.");
        exit(EXIT_FAILURE);
    }
}


int yaml_helper::length()
{  // is used for count the yaml-queue length

    if (yaml_node_context_.IsDefined()) {
      int count = 0;       // 遍历映射的第一层 有效键
      for (auto it = yaml_node_context_.begin(); it != yaml_node_context_.end(); ++it) {
        if (it->second.IsDefined() && (!it->second.IsNull()) ) {
          ++count;
        }
      }
      return count;
    } else if (!yaml_file_path_.empty()) {
      yaml_node_context_ = YAML::LoadFile(yaml_file_path_);
      return length();
    } else {
      perror("you've not get yamlfile or yamlcontext first.");
      exit(EXIT_FAILURE);
    }
    return 0;
}