#include "yaml_helper.hpp"


int main()
{
  std::filesystem::path currentPath = std::filesystem::current_path();
  yaml_helper testt;
  testt.setYamlFile("../test/test_log.yaml");
  YAML::Node node = testt.yaml_value();
  std::string val_str = testt.yaml_value_str();
  std::cout << val_str << std::endl;
  std::cout << testt.length() << std::endl;
  testt.write_into_file("test_log.yaml");

}
