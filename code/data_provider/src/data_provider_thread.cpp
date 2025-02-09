#include "data_provider_thread.hpp"
#include "yaml_helper.hpp"


namespace SeedPoolServicer
{
void
DataProviderThread::update_prepared_seeds()
{
  /* @brief 范例函数：update_prepared_seeds
    *  override的时候，务必按照以下【】要求进行改写，
    *  且要保证每个【】模块的顺序与此模板一致
    */
    // 【可改项】 防止prepared_seed过多膨胀，这里可以添加预备种子上限个数检查，比如：
    if(prepared_seeds_.size()>=5) return ; 
    // 【必写项】 pick种子  // pick : from seed_pool
    std::string yaml_data_fp = pickSeed().seedData;

    // 【可选项】 mutator逻辑：
    yamler_.setYamlFile(yaml_data_fp);
    YAML::Node seed_data = yamler_.yaml_value();
    yaml_mutate(seed_data);

    // write mutated_data -> prepared_seed_ (new yaml-file)
    std::string tmp_fp =
        (name_seed_folder_ / ("mutated_seed" + stamp()) ).string();

    yamler_.setYamlNodeContext(seed_data);
    yamler_.write_into_file(tmp_fp);
    // 【必写项】 
    push_prepared_seed(tmp_fp);
    return;
}



}
