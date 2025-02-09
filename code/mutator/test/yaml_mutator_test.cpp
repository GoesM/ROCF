#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

#include "defs.h"
#include "yaml_mutator.hpp"
#include "knobs.h"

namespace trooper
{
void Test()
{
    // give your yaml-data 
    std::string fp;
    std::cout << "input your yaml file's absolute path: " << std::endl;
    std::cin >> fp; fp.erase(std::remove(fp.begin(), fp.end(), '\''), fp.end());
    YAML::Node yaml = YAML::LoadFile(fp);  // give yaml-data


    // 0> Declaration: YamlMutator mutator(seed,my_knobs,&yaml);
    YamlMutator mutator;
    // usage: 

    for(int i=1;i<=5;i++)
    {
        YAML::Node new_yaml = mutator.mutate(
            &yaml,  // pointer to your yaml-data
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
        // demo>
        std::ofstream fout2("output_value_mutate" + std::to_string(i) +  ".yaml");
        fout2 << new_yaml;
    }
    return;
}
} // namespace trooper

int main()
{
    trooper::Test();
    return 0;
}
