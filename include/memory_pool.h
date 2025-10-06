#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <cstddef>
#include <mutex>
#include <iostream>

class MemoryPool {
public:
    struct Stats {
        size_t allocated = 0;      // 总分配字节数
        size_t freed = 0;          // 总释放字节数
        long long current_usage = 0; // 当前使用字节数（允许负数检查）
        size_t pool_size = 0;
    };

    explicit MemoryPool(size_t size);
    ~MemoryPool();

    void* allocate(size_t size);
    void deallocate(void* ptr);

    Stats getStats() const;
    void resetStats(); // 新增：重置统计信息

private:
    struct Block {
        size_t size;
        Block* next;
    };

    char* pool;
    Block* freeList;
    size_t poolSize;
    mutable std::mutex mtx;
    Stats stats;
};

#endif
