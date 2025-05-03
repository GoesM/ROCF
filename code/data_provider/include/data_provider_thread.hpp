#ifndef DATA_PROVIDER_THREAD_HPP
#define DATA_PROVIDER_THREAD_HPP

// mutate file-context
// all seed context is in file
#include "file_seed_pool_thread.hpp"
#include "yaml_mutator.hpp"

#include "configer.hpp"
#include "yaml_helper.hpp" 
namespace SeedPoolServicer
{


class DataProviderThread : public FileSeedPoolThread
{
public:
  DataProviderThread(const std::string & seed_pool_name)
  : DataProviderThread(seed_pool_name, true) {}

  DataProviderThread(const std::string & seed_pool_name, const bool & ave_possibility)
  : FileSeedPoolThread(seed_pool_name, ave_possibility)
  {}

  // for settings
  void set_max_mutate_times(const int & x) {max_mutate_times_ = x;}
  // for "adaptive mutation depth" 
  void add_mutate_times(const int & x)
  {
    mutate_times_ += x;
    if (mutate_times_ < 0) {mutate_times_ = 1;}
    mutate_times_ = std::min(mutate_times_, max_mutate_times_);
  }
  ~DataProviderThread() {}

private:
  void update_prepared_seeds() override;

private:  // something for mutator...
  int max_mutate_times_{10000};   // it's suggested, because yaml-mutator could finish 10k times in one second
  int mutate_times_{10};          // nav2_param.yaml, slam-toolbox.yaml
  
  void yaml_mutate(YAML::Node & yaml_data)
  {
    yaml_data = mutator.mutate(
        &yaml_data,  // pointer to your yaml-data
        2, // mutation depth
        // list-mutation-balance:
        std::array<uint8_t,7>{
        1, // field-change: value mutation
        1, // field-change: random value
        1, // field-change: value from dictionary
        1, // field-exchange
        1, // field-insertion: random value
        1, // field-insertion: from dictionary
        1  // field-deletion
        },
        // binary-mutation-balance"
        std::array<uint8_t,7>{
        1, // EraseBytes_
        1, // FlipBits_
        1, // SwapBytes_
        1, // ChangeBytes_
        1, // ChangeBytesByDictionary_
        1, // InsertBytes_
        1  // InsertBytesByDictionary_
        }
    );
  }

private: // plugins
    yaml_helper yamler_;
    YamlMutator mutator;

}



};
#endif
