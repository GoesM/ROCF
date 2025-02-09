#ifndef YAML_MUTATOR_BASE_HPP
#define YAML_MUTATOR_BASE_HPP
/*
  @brief 提供基本的函数
*/
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <functional>
#include <cstdint>

#include "defs.h"
#include "mutator.h"
#include "knobs.h"

namespace trooper
{

// 生成一个随机数生成器
static std::random_device rd;
static std::mt19937 gen(rd());


enum class ValueType
{
  Unknown,
  String,
  Integer,
  Double,
  Bool,
  List,
  SubNode,
  Empty
};

//--------------------数据类型解析----------------------//
template <typename KeyType>
ValueType getValueType(const YAML::Node *node, const KeyType &key)
{
    // 确保 KeyType 只能是 int 或 std::string
    static_assert(
        std::is_same<KeyType, int>::value || std::is_same<KeyType, std::string>::value,
        "KeyType must be int or std::string"
    );

    if (!(*node)[key].IsDefined()) {return ValueType::Empty;}
    if ((*node)[key].IsNull()) {return ValueType::Empty;}
    if ((*node)[key].IsSequence()) {return ValueType::List;}
    if ((*node)[key].IsMap()) {return ValueType::SubNode;}
    if (!(*node)[key].IsScalar()) {return ValueType::Unknown;}
    // int or double or bool or string
    try {
        int value = (*node)[key].template as<int>();
        return ValueType::Integer;

    } catch (const YAML::BadConversion & e) {
        try {
        double value = (*node)[key].template as<double>();
        return ValueType::Double;
        } catch (const YAML::BadConversion & e) {
        try {
            // try to explain node as bool
            bool value = (*node)[key].template as<bool>();
            return ValueType::Bool;

        } catch (const YAML::BadConversion & e) {
            return ValueType::String;
        }
        }
    }

    // ?
    return ValueType::Unknown;
}

static bool isInteger(double value)
{
  // 将 double 值转换为整数
  int intValue = static_cast<int>(value);
  // 检查转换后的值是否等于原始 double 值
  return value == intValue;
}



//--------------convert type into/from 2-binary----------------------------//
// convert doulble, uint8_t
ByteArray DoubleToUint8(double value);
double Uint8ToDouble(ByteSpan bytes);
// convert int, uint8_t
ByteArray IntToUint8(int value);
int Uint8ToInt(ByteSpan bytes);
// convert bool, uint8_t
ByteArray BoolToUint8(bool value);
bool Uint8ToBool(ByteArray byte);
//--------------convert type into/from 2-binary----------------------------//
ByteArray DoubleToUint8(double value)
{
  ByteArray result(sizeof(double));
  std::memcpy(result.data(), &value, sizeof(double));
  return result;
}

ByteArray IntToUint8(int value)
{
  ByteArray result(sizeof(int));
  std::memcpy(result.data(), &value, sizeof(int));
  return result;
}

ByteArray BoolToUint8(bool value)
{
  ByteArray result(sizeof(bool));
  std::memcpy(result.data(), &value, sizeof(bool));
  return result;
}


// double Uint8ToDouble(ByteSpan bytes)
// {
//   double value;
//   std::memcpy(&value, bytes.data(), sizeof(double));
//   return value;
// }
double Uint8ToDouble(ByteSpan data)
{
  std::array<uint8_t, sizeof(double)> buffer{};   // Initialize with zeros
  size_t copySize = std::min(data.size(), sizeof(double));

  // Copy bytes from data to buffer
  std::memcpy(buffer.data() + sizeof(double) - copySize, data.data(), copySize);

  double result;
  std::memcpy(&result, buffer.data(), sizeof(double));
  return result;
}
// int Uint8ToInt(ByteSpan bytes)
// {
//   int value;
//   std::memcpy(&value, bytes.data(), sizeof(int));
//   return value;
// }
int Uint8ToInt(ByteSpan data)
{
  std::array<uint8_t, sizeof(int)> buffer{};   // Initialize with zeros

  size_t copySize = std::min(data.size(), sizeof(int));

  // Copy bytes from data to buffer
  std::memcpy(buffer.data() + sizeof(int) - copySize, data.data(), copySize);

  int result;
  std::memcpy(&result, buffer.data(), sizeof(int));
  return result;
}
// bool Uint8ToBool(ByteArray bytes)
// {
//     bool value;
//     std::memcpy(&value, bytes.data(),sizeof(bool));
//     return value;
// }
bool Uint8ToBool(ByteArray data)
{
  std::array<uint8_t, sizeof(bool)> buffer{};   // Initialize with zeros

  size_t copySize = std::min(data.size(), sizeof(bool));

  // Copy bytes from data to buffer
  std::memcpy(buffer.data() + sizeof(bool) - copySize, data.data(), copySize);

  bool result;
  std::memcpy(&result, buffer.data(), sizeof(bool));
  return result;
}

}


#endif