#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>


#include <iostream>
#include <thread>
// 测试函数
void func(int id) {
    
    if(id<=0) return ; 
    else func(id-1);
    std::cout << "func(): " << id <<  " ------------------------------\n";
    return;
}

// 测试函数
void t1_func(int id) {
    
    if(id<=0) return ; 
    else t1_func(id-1);
    std::cout << "t1_func(): " << id <<  " ------------------------------\n";
    return;
}



int main() {
    std::thread t1(t1_func,100);
    func(100);
    std::thread t2(func,100);
    // std::thread t3(func,100);

    
    t1.join();
    t2.join();
    // t3.join();
        
    return 0;
}