#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <execinfo.h>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <random>
#include <functional>
#include <fstream>

#ifndef thread_delayer_hook_global
#define thread_delayer_hook_global
const int HASH_TABLE_SIZE = 1e5;
struct SharedDataROCF    // 定义共享内存中的数据结构
{
    long long delay_time[HASH_TABLE_SIZE]; // from input_data.yaml 
};

// Call stack information
struct CallStackInfo {
    uint64_t start_time;
    uint64_t end_time;
    uint64_t func_hash;
    pthread_t thread_id;

    // 用于PairSequence前缀关系
    __attribute__((no_sanitize("coverage")))
    bool operator==(const CallStackInfo& other) const {
        return start_time == other.start_time &&
               end_time == other.end_time &&
               func_hash == other.func_hash &&
               thread_id == other.thread_id;
    }
};
// call stack pair
struct CallStackPair{
    CallStackInfo LCS; // lifecycle-related call stack
    CallStackInfo GCS; // general call stack

    // 用于PairSequence前缀关系，因此只判断func_hash一致性
    __attribute__((no_sanitize("coverage")))
    bool operator==(const CallStackPair& other) const {
        return LCS.func_hash == other.LCS.func_hash && GCS.func_hash==other.GCS.func_hash;
    }
};
// Lifecycle pair sequence
struct PairSequence {
    std::queue<CallStackPair> pair_sequence;

    // 判断自己是不是另一个PairSequence的前缀
    __attribute__((no_sanitize("coverage")))
    bool is_prefix_of(const PairSequence& other) const {
        // 创建本地副本以操作队列而不修改原始数据
        std::queue<CallStackPair> this_copy = pair_sequence;
        std::queue<CallStackPair> other_copy = other.pair_sequence;

        // 检查当前队列是否是`other_copy`的前缀
        while (!this_copy.empty() && !other_copy.empty()) {
            if (!(this_copy.front() == other_copy.front())) {
                return false; // 任意一对不匹配，返回false
            }
            this_copy.pop();
            other_copy.pop();
        }

        // 如果自身耗尽，则是前缀
        return this_copy.empty();
    }
};


const int PORT = 5713;
extern const char *SERVER_ADDRESS;
extern int sock ;
extern struct sockaddr_in serv_addr;
extern bool SOCKET_CONNECTION_SUCCESS;
#endif


#ifndef HOOK_DEF_HPP
#define HOOK_DEF_HPP
void deserialize_CallStackInfo(const char *buffer, CallStackInfo &data);
bool initialize_client();
#endif