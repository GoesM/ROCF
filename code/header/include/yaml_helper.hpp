#ifndef YAML_FILE_H
#define YAML_FILE_H

#include <filesystem>
#include <fstream> 
#include <iostream>
#include <yaml-cpp/yaml.h>

/*  @brief yaml-provider for FUZZ_ROUND
    *  provide YAML::NOde for FUZZ_ROUND
    *  provide .yaml file-path for FUZZ_ROUND
    */
class yaml_helper
{
public:
  // set log-file-path in constructor
  yaml_helper() {}
  ~yaml_helper() {}

  // set context in setYamlFile or setYamlContext
  void setYamlFile(const std::string & yaml_file_path)
  {
    yaml_file_path_ = yaml_file_path;
    yaml_node_context_ = YAML::LoadFile(yaml_file_path);
    yaml_str_context_ = YAML::Dump(yaml_node_context_);
  }
  void setYamlNodeContext(const YAML::Node & yaml_context)
  {
    yaml_node_context_ = yaml_context;
    yaml_str_context_ = YAML::Dump(yaml_node_context_);

  }
  void setYamlStrContext(const std::string & yaml_context)
  {
    yaml_str_context_ = yaml_context; 
    try {
      yaml_node_context_ = YAML::Load(yaml_str_context_);
    } catch (const YAML::ParserException& e) {
        std::cerr << "Error parsing YAML: " << e.what() << std::endl;
        yaml_node_context_ = YAML::Node();
        yaml_str_context_ = "";
    }
  }

  // return value as YAML::Node
  YAML::Node yaml_value();
  std::string yaml_value_str();

  // record into the file 
  void write_into_file(const std::string & log_file_path_);

  // return the length of a YAML::Node
  int length();

private:
  // read context
  std::string yaml_file_path_{""};
  YAML::Node yaml_node_context_;
  std::string yaml_str_context_{""};
};


#endif
