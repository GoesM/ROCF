#include "yaml_mutator.hpp"


namespace trooper
{

int YamlMutator::getRandomKey_YamlList(YAML::Node * node)
{
    std::uniform_int_distribution<> dis(0, node->size() - 1);
    int index = dis(gen);
    return index;
}
std::string YamlMutator::getRandomKey_YamlNode(YAML::Node * node)
{   
    // 确保 KeyType 只能是 int
    // static_assert(std::is_same<KeyType, std::string>::value, "KeyType must be std::string for a map");
    std::vector<std::string> keys_;
    for (auto it = node->begin(); it != node->end(); ++it) {
        keys_.push_back(it->first.as<std::string>());
    }
    // 生成一个随机的键的索引
    std::uniform_int_distribution<int> distribution(0, keys_.size() - 1);
    int index = distribution(gen);
    // 返回对应key
    return keys_[index];
}

bool YamlMutator::mutate_yaml(YAML::Node * node)
{
    if (node->IsSequence()) {
        return mutate_list(node);
    }
    else {
        std::string key = getRandomKey_YamlNode(node);
        return mutate_item(node, key);
    }
}


// ----------------------mutation for list ------------------------ //
bool YamlMutator::mutate_list(YAML::Node * node) // node->isSeq() == True
{
    if (node->size() <= 0) { return false;} // only mutate when not empty
    // list-mutator:
    size_t id = list_mutator_knobs_.Choose(list_mutator_knobs_ids_, rng_());
        // std::cout << "[debug]: list choose id:" << id << std::endl;
    if (id == 0) {return list_value_mutate(node);} // 字段变异
    if (id == 1) {return list_value_mutate(node);} // @todo 随机值-字段
    if (id == 2) {return list_value_mutate(node);} // @todo 字典值-字段
    if (id == 3) {return list_swap_item(node);}    // 字段交换
    if (id == 4) {return list_insert_item(node);}  // @todo 随机插入
    if (id == 5) {return list_insert_item(node);}  // @todo 字典插入
    if (id == 6) {return list_erase_item(node);}  // 字段删除
    // error
    std::cerr << "something wrong happened in randmom() in yaml_mutator" << std::endl;
    exit(EXIT_FAILURE);
}
// 字段擦除
bool YamlMutator::list_erase_item(YAML::Node * node)
{
    if (node->size() <= 0) { return false; } // only erase when not empty
    int index = getRandomKey_YamlList(node);
    node->remove(index);
    return true;
}
// 字段插入 （暂时使用：字段重放）
bool YamlMutator::list_insert_item(YAML::Node * node)
{
    if (node->size() <= 0) { return false; } // only erase when not empty
    // pick from other item
    int index = getRandomKey_YamlList(node);
    YAML::Node temp = YAML::Clone((*node)[index]); // use `YAML::Clone` instead of `=` to avoid reference
    // std::string temp = (*node)[index].as<std::string>(); // copy as string, to avoid reference
    node->push_back(temp);
    return true;
}
// 字段交换
bool YamlMutator::list_swap_item(YAML::Node * node)
{
    if (node->size() <= 1) {
        return false;                       // only swap when not <= 1
    }
    // random pick two
    int index1 = getRandomKey_YamlList(node);
    int index2 = getRandomKey_YamlList(node);
    if(index1 == index2) return false;
    // 交换元素位置
    YAML::Node temp1 = YAML::Clone( (*node)[index1]);  // use `YAML::Clone` instead of `=` to avoid reference
    YAML::Node temp2 = YAML::Clone( (*node)[index2]);  // use `YAML::Clone` instead of `=` to avoid reference
    (*node)[index1] = temp2;
    (*node)[index2] = temp1;
    return true;
}
// 列表的 字段变异
bool YamlMutator::list_value_mutate(YAML::Node * node)
{
    int key = getRandomKey_YamlList(node);
    return mutate_item(node, key);
}
}
