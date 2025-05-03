#include "queue_usr_input.hpp"
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <thread>

int main()
{
  std::cout << "launch navigation2 first" << std::endl;
  std::cout << " if you've launched nav2, input anything" << std::endl;
  std::getchar();

  std::filesystem::path currentPath = std::filesystem::current_path();
  set_env("USR_INPUT_LOG_DIR", currentPath.string());
  queue_usr_input user("test");

  //provide a queue in YAML::Node
  yaml_file yaml_helper("");
  yaml_helper.setYamlFile("../queue_usr_input.yaml");
  YAML::Node command_queue = yaml_helper.ProvideYamlNode();

  // fork - launch
  user.launch_usr_fork(command_queue);

  std::this_thread::sleep_for(std::chrono::seconds(5));
  // early shutdown
  user.shutdown();

  return 0;
}
