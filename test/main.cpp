#include "memory_pool.h"
#include <iostream>
#include <thread>
#include <vector>

void worker(MemoryPool& pool, int id) {
    int* a = static_cast<int*>(pool.allocate(sizeof(int)));
    int* b = static_cast<int*>(pool.allocate(sizeof(int)));
    *a = id * 100 + 1;
    *b = id * 100 + 2;

    std::cout << "Thread " << id << " -> a: " << *a << ", b: " << *b << "\n";

    pool.deallocate(a);
    pool.deallocate(b);
}

int main() {
    MemoryPool pool(1024);
    pool.resetStats(); // ✅ 确保每次运行统计清零

    std::vector<std::thread> threads;
    for (int i = 1; i <= 4; i++) {
        threads.emplace_back(worker, std::ref(pool), i);
    }

    for (auto& t : threads) t.join();

    auto stats = pool.getStats();
    std::cout << "Total Allocated: " << stats.allocated
              << ", Total Freed: " << stats.freed
              << ", Current Usage: " << stats.current_usage
              << ", Pool Size: " << stats.pool_size << "\n";
}
