#include "hook_communicator.hpp"

  
  
void HookCommunicator::analysis() {
    // 找到的lifecycle-related call stack pairs
    std::vector<CallStackPair> call_stack_pairs;
    // 新覆盖的 lifecycle_pair_sequences
    std::vector<PairSequence> tmp_covered_lifecycle_pair_sequences_;

    func_schedule_ = call_stack_info_collector_.result();
    /* @debug */
    std::cout << "Call stack info's size:" << func_schedule_.size() << std::endl;

    // Step 1: Identify lifecycle-related call stacks
    std::vector<CallStackInfo> lifecycle_related_callstacks;
    for (const auto& info : func_schedule_) {
        if (lifecycle_func_hash_.empty()){
            lifecycle_related_callstacks.push_back(info);
        }
        else if (std::find(lifecycle_func_hash_.begin(), lifecycle_func_hash_.end(), info.func_hash) != lifecycle_func_hash_.end()) {
            lifecycle_related_callstacks.push_back(info);
        }
    }
    /* @debug */
    std::cout << "lifecycle_related_callstacks' size:" << lifecycle_related_callstacks.size() << std::endl;


    // Step 2: Construct call stack pairs
    for (const auto& LCS : lifecycle_related_callstacks) {
        for (const auto& GCS : func_schedule_) {
            if (is_concurrent(LCS, GCS)) {
                CallStackPair pair{LCS, GCS};
                call_stack_pairs.push_back(pair);
            }
        }
    }
    /* @debug */
    std::cout << "lifecycle_related_callstack pairs' size:" << call_stack_pairs.size() << std::endl;


    // Step 3: Combine call stack pairs into lifecycle pair sequences
    for (const auto& CS_pair : call_stack_pairs) {
        bool inserted = false;

        // 尝试将当前 `CS_pair` 插入到现有的 `tmp_covered_lifecycle_pair_sequences_` 中
        for (auto& sequence : tmp_covered_lifecycle_pair_sequences_) {
            // 获取当前序列的引用队列（只读操作）
            std::queue<CallStackPair> sequence_copy = sequence.pair_sequence;
            
            // 检查是否可以插入到现有的 `sequence` 中
            bool can_insert = false;
            while (!sequence_copy.empty()) {
                const auto& existing_pair = sequence_copy.front();

                // 条件：相同的LCS，且GCS线程一致
                if (existing_pair.LCS == CS_pair.LCS &&
                    existing_pair.GCS.thread_id == CS_pair.GCS.thread_id) {
                    can_insert = true;
                    break;
                }
                sequence_copy.pop();
            }

            if (can_insert) {
                // 按照GCS的start time顺序插入到对应的队列中
                std::queue<CallStackPair> updated_queue;
                bool inserted_in_order = false;

                while (!sequence.pair_sequence.empty()) {
                    const auto& existing_pair = sequence.pair_sequence.front();

                    if (!inserted_in_order && CS_pair.GCS.start_time < existing_pair.GCS.start_time) {
                        updated_queue.push(CS_pair);
                        inserted_in_order = true;
                    }

                    updated_queue.push(existing_pair);
                    sequence.pair_sequence.pop();
                }

                // 如果还未插入，则插入到末尾
                if (!inserted_in_order) {
                    updated_queue.push(CS_pair);
                }

                sequence.pair_sequence = std::move(updated_queue);
                inserted = true;
                break;
            }
        }

        // 如果没有插入到任何序列中，则创建一个新的序列
        if (!inserted) {
            PairSequence new_sequence;
            new_sequence.pair_sequence.push(CS_pair);
            tmp_covered_lifecycle_pair_sequences_.push_back(std::move(new_sequence));
        }
    }

    // Step 4: 合并lifecycle pair sequences集合，并去重 

    // 对于每个seq判断，如果其不是coverd_lifecycle_pair_seqeunces中任何序列的前缀，则加入，并将是它的前缀的都删除；否则不加入。
    std::vector<PairSequence> new_covered_sequences;
    for (const auto& new_seq : tmp_covered_lifecycle_pair_sequences_) {
        bool is_covered = false;

        for (auto it = covered_lifecycle_pair_sequences_.begin(); it != covered_lifecycle_pair_sequences_.end();) {
            if (new_seq.is_prefix_of(*it)) {
                // 是别人的前缀，跳过
                is_covered = true;
                break;
            } else if (it->is_prefix_of(new_seq)) {
                // 去掉它的前缀
                it = covered_lifecycle_pair_sequences_.erase(it);
            } else {
                ++it;
            }
        }

        if (!is_covered) {
            new_covered_sequences.push_back(new_seq);
        }
    }
    // 将新覆盖的seq加入
    for (const auto& seq : new_covered_sequences) {
        covered_lifecycle_pair_sequences_.push_back(seq);
    }

    // 更新总的覆盖率结果
    total_con_cov_ = covered_lifecycle_pair_sequences_.size();
    con_cov_up_ = new_covered_sequences.size();
}
