#ifndef YAML_MUTATOR_HPP
#define YAML_MUTATOR_HPP
#include "yaml_mutator_struct.hpp"


namespace trooper
{

class YamlMutator: public YamlMutatorStruct
{

public:
    YamlMutator()
    : YamlMutator(time(0)){}
    YamlMutator(uintptr_t seed) // 提供一个随机数种子
    : YamlMutatorStruct(seed){}
    
    YAML::Node mutate(  // 会直接修改原数据，同时也会返回YAML值
        YAML::Node * yaml_ptr,   // 被变异的yaml数据 （指针） 
        int mutate_depth = 1,    // 变异深度（被变异的次数）
        std::array<uint8_t,7> list_knob_value = defaulted_knobs_value,
            // 策略平衡 （每个 list-mutator变异策略被使用的概率）
        std::array<uint8_t,7> item_knob_value = defaulted_knobs_value 
            // 策略平衡（每个 二进制mutator变异策略被使用的概率）
    ){return API_mutate(yaml_ptr,mutate_depth,list_knob_value,item_knob_value);}



private: // yaml::node mutate
    // mutate any YAML::Node
    bool mutate_yaml(YAML::Node * node);
    Rng rng_;

private:  // list mutate
    bool mutate_list(YAML::Node * node); // 列表变异
    bool list_erase_item(YAML::Node * node); // 字段擦除
    bool list_insert_item(YAML::Node * node); // 字段插入 （暂时使用：字段重放）
    bool list_swap_item(YAML::Node * node);  // 字段交换
    bool list_value_mutate(YAML::Node * node);  // 列表的 字段变异  

private:
    // 随机选择一个字段的下标（key）
    int getRandomKey_YamlList(YAML::Node * node);
    std::string getRandomKey_YamlNode(YAML::Node * node);

// -----------------------------template 需要在hpp中实现。--------------------------------//
private:  // 对于下标为key的字段进行变异
    template <typename KeyType>
    bool mutate_item(YAML::Node * node, KeyType key)
    {
        // 确保 KeyType 只能是 int 或 std::string
        static_assert(
            std::is_same<KeyType, int>::value || std::is_same<KeyType, std::string>::value,
            "KeyType must be int or std::string"
        );

        // declaration
        bool flag = false;
        double ls;
        ByteArray tmp_aggr_data_;
        YAML::Node tmp_node;
        // 字段类型解析
        ValueType key_type = getValueType<KeyType>(node, key);
        // 基于语法变异
        switch (key_type) {
        case ValueType::String:
            // no mutate for string
            return false;
        case ValueType::Integer:
            tmp_aggr_data_ = IntToUint8((*node)[key].template as<int>());     // - convert
            flag = mutator_.Mutate(tmp_aggr_data_);                  // - mutate
            (*node)[key] = Uint8ToInt(tmp_aggr_data_);               // - convert
            return flag;

        case ValueType::Double:
            tmp_aggr_data_ = DoubleToUint8((*node)[key].template as<double>());  // - convert
            flag = mutator_.Mutate(tmp_aggr_data_);                     // - mutate
            ls = Uint8ToDouble(tmp_aggr_data_);                         // - convert
            if (isInteger(ls)) {
            (*node)[key] = std::to_string(ls);                    // to avoid 0.0 into 0
            } else {(*node)[key] = ls;}
            return flag;

        case ValueType::Bool:
            tmp_aggr_data_ = BoolToUint8((*node)[key].template as<bool>());    // - convert
            flag = mutator_.Mutate(tmp_aggr_data_);                   // - mutate
            (*node)[key] = Uint8ToBool(tmp_aggr_data_);               // - convert
            return flag;

        // others:
        case ValueType::List:
            tmp_node = (*node)[key];            // - convert
            flag = mutate_list(&tmp_node);      // - mutate
            (*node)[key] = tmp_node;            // - convert
            return flag;

        case ValueType::SubNode:
            tmp_node = (*node)[key];            // - convert
            flag = mutate_yaml(&tmp_node);      // - mutate
            (*node)[key] = tmp_node;            // - convert
            return flag;

        // avoid error
        case ValueType::Unknown:
            return false;
        case ValueType::Empty:
            return false;
        }
        // error
        std::cerr << "something wrong happened in randmom() in yaml_mutator" << std::endl;
        exit(EXIT_FAILURE);
    }
};

} // namespace trooper


#endif
