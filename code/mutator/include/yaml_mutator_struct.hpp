#ifndef YAML_ITEM_MUTATOR_HPP
#define YAML_ITEM_MUTATOR_HPP
#include "yaml_mutator_base.hpp"


namespace trooper
{
const std::array<uint8_t,7> defaulted_knobs_value = std::array<uint8_t,7>({1,1,1,1,1,1,1});
const std::array<size_t,7> defaulted_knobs_ids = std::array<size_t,7>({1,1,1,1,1,1,1});
class YamlMutatorStruct   // 基础架构
{

public:
    YamlMutatorStruct()
    : YamlMutatorStruct(time(0)){}

    YamlMutatorStruct(uintptr_t seed) // 随机数种子
    : mutator_(seed, item_mutator_knobs_),
      list_mutator_knobs_ids_{
        list_mutator_knobs_.NewId("field-change: value mutation"),
        list_mutator_knobs_.NewId("field-change: random value"),
        list_mutator_knobs_.NewId("field-change: value from dictionary"),
        list_mutator_knobs_.NewId("field-exchange"),
        list_mutator_knobs_.NewId("field-insertion: random value"),
        list_mutator_knobs_.NewId("field-insertion: from dictionary"),
        list_mutator_knobs_.NewId("field-deletion"),
      }
    {
    }

protected:
    YAML::Node API_mutate(
        YAML::Node * yaml_ptr,   // 被变异的yaml数据 （指针） // 会直接修改原数据
        int mutate_depth = 1,    // 变异深度（被变异的次数）
        std::array<uint8_t,7> list_knob_value = defaulted_knobs_value,
            // 策略平衡 （每个 list-mutator变异策略被使用的概率）
        std::array<uint8_t,7> item_knob_value = defaulted_knobs_value 
            // 策略平衡（每个 二进制mutator变异策略被使用的概率）
    )
    {
        // 策略概率
        // update
        item_mutator_knobs_value_ = item_knob_value;
        item_mutator_knobs_.Set(item_knob_value);  // 设置各个策略被选择的概率
        // udpate
        list_mutator_knobs_value_ = list_knob_value;
        list_mutator_knobs_.Set(list_knob_value);  // 设置各个策略被选择的概率 

        // mutate主逻辑
        set_corpus(yaml_ptr);   // 设置被变异的目标
        while(mutate_depth--) 
        {
            mutate_once(); // 根据变异深度进行变异
        }
        return *corpus_;
    }

    bool mutate_once()
    {
        for (int i = 1; i <= 15; i++) {
            if (mutate_yaml(corpus_))
            {return true;}
        }
        return false;
    }

private:  // 设置变异目标
    // Update yaml corpus/seed. !!!Though a scalar node is acceptable,
    // it's not a valid yaml format, which should be avoided.
    void set_corpus(YAML::Node * new_corpus)
    {
        if (new_corpus == nullptr) {return;}
        // std::cout << *new_corpus << std::endl;
        if (!new_corpus) {
        std::cerr << "[yaml_mutator][ERROR]: please provide a data as input";
        __builtin_trap();
        }
        if (!new_corpus->IsMap() && !new_corpus->IsSequence()) {
        std::cerr << "[yaml_mutator][ERROR]: only YAML::Map or YAML::List is supported as input";
        __builtin_trap();
        }
        corpus_ = new_corpus;
    }

protected: // item_mutator
    Knobs item_mutator_knobs_;
    std::array<size_t, 7> item_mutator_knobs_ids_{defaulted_knobs_ids};
    std::array<uint8_t,7> item_mutator_knobs_value_{defaulted_knobs_value};
    Mutator mutator_{Mutator(time(0),item_mutator_knobs_)};  // bytearray mutator

protected: // list_mutator
    Knobs list_mutator_knobs_;
    std::array<size_t, 7> list_mutator_knobs_ids_{defaulted_knobs_ids};
    std::array<uint8_t,7> list_mutator_knobs_value_{defaulted_knobs_value};
    // target data to be mutated
    YAML::Node * corpus_{nullptr};  // yaml corpus

protected:
    virtual bool mutate_yaml(YAML::Node * node) = 0;  // 在yaml_mutator中实现




};

} // namespace trooper


#endif
