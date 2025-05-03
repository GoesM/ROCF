#ifndef CONFIGER_HPP
#define CONFIGER_HPP

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <yaml-cpp/yaml.h>

namespace CONFIGER{


// --------------private-----------//
// 通用模板函数，用于将字符串转换为指定类型
template<typename T>
T convert_to_type(const std::string & str);
// 通用模板函数，用于将值转换为字符串
template<typename T>
std::string convert_to_string(const T & value);

// 
class yaml_env
{
public:
  /* @brief env from yaml
  * read yaml and set it into env
  */
  yaml_env(const std::string & YAML_FILE_PATH)
  : YAML_FILE_PATH_(YAML_FILE_PATH)
  {
    context_ = YAML::LoadFile(YAML_FILE_PATH_);
  }

  ~yaml_env(){};

  void store_into_env()
  {
    for (const auto & entry : context_) {
      if (entry.second.IsScalar()) {
        const std::string key = entry.first.as<std::string>();
        const std::string value = entry.second.as<std::string>();
        setenv(key.c_str(), value.c_str(), 1);        // 覆盖已存在的同名环境变量
      }
      // 如果有嵌套结构，可以递归调用 store_into_env
      // 这里的示例仅处理标量值
    }
  }


  template<typename T>
  T read_value(const std::string & key)
  {
    if (!key_exist(key)) {
      return T();
    } else {
      return context_[key].as<T>();
    }
  }


private:
  YAML::Node context_;
  std::string YAML_FILE_PATH_;
  bool key_exist(const std::string & key)
  {
    if (!context_[key]) {
      printf("Key \"%s\" doesn't exist.\n", key.c_str());
      return false;
    }
    return true;
  }
};

// ---------------------------public ----------------------//
// API: 读取环境变量的通用模板函数
template<typename T>
T read(const std::string & env_var)
{
  const char * value = std::getenv(env_var.c_str());

  if (value != nullptr) {
    if constexpr (std::is_same_v<T, bool>) {
      std::string lowercaseStr;
      // Convert the string to lowercase
      for (const char * ptr = value; *ptr != '\0'; ++ptr) {
        lowercaseStr += std::tolower(*ptr);
      }
      // 将字符串转换为布尔值
      if (lowercaseStr == "true") {
        return true;
      } else if (lowercaseStr == "false") {
        return false;
      } else {
        // Throw an error if the string is neither "true" nor "false"
        throw std::invalid_argument(
                "env_value " + env_var + " is Invalid boolean string: " + lowercaseStr);
      }
    } else {
      return convert_to_type<T>(value);
    }
  } else {
    return T(); // 对于其他类型，默认值
  }
}

// API: 通用模板函数，用于设置环境变量
template<typename T>
void set(const std::string & env_var, const T & value)
{
  std::string strValue = convert_to_string(value);
  setenv(env_var.c_str(), strValue.c_str(), 1);    // 覆盖已存在的同名环境变量
}

// API: 从yaml文件中读config
template<typename T>
T read_from_yamlfile(const std::string & YAML_FILE_PATH, const std::string & env_var)
{
    yaml_env helper = yaml_env(YAML_FILE_PATH);
    return helper.read_value<T>(env_var);
}

// API: 根据yaml文件设置config
void set_from_yamlfile(const std::string & YAML_FILE_PATH)
{
    yaml_env helper = yaml_env(YAML_FILE_PATH);
    helper.store_into_env();
    return;
}



// 通用模板函数，用于将字符串转换为指定类型
template<typename T>
T convert_to_type(const std::string & str)
{
  T value;
  std::istringstream stream(str);
  stream >> value;
  return value;
}
// 通用模板函数，用于将值转换为字符串
template<typename T>
std::string convert_to_string(const T & value)
{
  std::ostringstream stream;
  stream << value;
  return stream.str();
}





};
#endif
