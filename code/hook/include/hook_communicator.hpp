
#include "shared_mem_provider.hpp"
#include "socket_server.hpp"


#ifndef hook_commnicator_HPP
#define hook_commnicator_HPP



class HookCommunicator {

public:
    HookCommunicator(){}
    ~HookCommunicator(){}

    void launch()
    {
        call_stack_info_collector_.launch();
        delayer_director_.open();
    }

    void update(YAML::Node & delayer_queue){
        // plugin update
        delayer_director_.update(delayer_queue);
        call_stack_info_collector_.clear();
        call_stack_info_collector_.update();
        // clear data
        func_schedule_.clear();
        con_cov_up_ = 0;
    }

   /* @todo lifecycle pair sequence calculation:
    * step1. Identify lifecycle-related callstacks: 
    *    遍历info queue, 如果其func_hash在lifecycle_func_hash_中，则是lifecycle-related callstack。
    * step2. Construct call stack pairs: 
    *    对每个遍历到的lifecycle-related call stack，在info queue中找与他符合is_concurrent函数判断的call stack，
    *    组成一个CallStackPair结构体变量
    * step3. Combine call stack pairs into lifecycle pair sequences。
    *    // 对于每个CS_pair都执行判断：
    *        // 如果在tmp_covered_lifecycle_pair_sequence中的某个sequence中出现了CS_pair.LCS
    *            // 且，这个sequence中的call_stack_pair.GCS和CS_pair.GCS在同一线程，
    *            // 则按照GCS的start time，将CS_pair按时序插入该sequence中
    *        // 否则，新建一个PairSeqeuence，并将该CS_pair存入。
    * step4. 集合合并 & 去重。
    *    // 对于每个seq判断，如果其不是coverd_lifecycle_pair_seqeunces中任何序列的前缀，则加入，并将是它的前缀的都删除；否则不加入。
    */
    void analysis();  
    
    void shutdown(){
        call_stack_info_collector_.shutdown();
        delayer_director_.close();
    }

    int total_cov_result(){return total_con_cov_;}
    int cov_up_result(){return con_cov_up_;}

private:
    // plugins
    SharedMemProvider delayer_director_; // fuzzer->hook
    SocketServer call_stack_info_collector_; // hook->fuzzer
    // collected data
    std::vector<CallStackInfo> func_schedule_;

private:
    // result
    std::vector<PairSequence> covered_lifecycle_pair_sequences_;
    int total_con_cov_{0};
    int con_cov_up_{0};

private: 
    bool is_concurrent(const CallStackInfo &a, const CallStackInfo &b) {
        return (a.thread_id!=b.thread_id) && (a.start_time < b.end_time) && (b.start_time < a.end_time) ;
    }
    std::vector<int> lifecycle_func_hash_{};
};


#endif