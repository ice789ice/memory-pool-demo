#include "memory_pool.h"
#include <cstdlib>
#include <new>
#include <cstring>
#include <algorithm>

MemoryPool::MemoryPool(size_t size) : poolSize(size) {
    pool = new char[size];
    freeList = reinterpret_cast<Block*>(pool);
    freeList->size = size - sizeof(Block);
    freeList->next = nullptr;

    stats.allocated = 0;
    stats.freed = 0;
    stats.current_usage = 0;
    stats.pool_size = poolSize;
}

MemoryPool::~MemoryPool() {
    delete[] pool;
}

void* MemoryPool::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mtx);

    size = (size + alignof(std::max_align_t) - 1) & ~(alignof(std::max_align_t) - 1);

    Block* prev = nullptr;
    Block* curr = freeList;

    while (curr) {
        if (curr->size >= size) {
            if (curr->size <= size + sizeof(Block)) {
                if (prev) prev->next = curr->next;
                else freeList = curr->next;
            } else {
                Block* newBlock = reinterpret_cast<Block*>(
                    reinterpret_cast<char*>(curr) + sizeof(Block) + size);
                newBlock->size = curr->size - size - sizeof(Block);
                newBlock->next = curr->next;

                if (prev) prev->next = newBlock;
                else freeList = newBlock;

                curr->size = size;
            }

            stats.allocated += size;
            stats.current_usage += size;

            return reinterpret_cast<char*>(curr) + sizeof(Block);
        }
        prev = curr;
        curr = curr->next;
    }

    throw std::bad_alloc();
}

void MemoryPool::deallocate(void* ptr) {
    if (!ptr) return;

    std::lock_guard<std::mutex> lock(mtx);

    Block* block = reinterpret_cast<Block*>(
        reinterpret_cast<char*>(ptr) - sizeof(Block));
    block->next = freeList;
    freeList = block;

    stats.freed += block->size;
    stats.current_usage -= block->size;
}

MemoryPool::Stats MemoryPool::getStats() const {
    std::lock_guard<std::mutex> lock(mtx);
    return stats;
}

void MemoryPool::resetStats() {
    std::lock_guard<std::mutex> lock(mtx);
    stats.allocated = 0;
    stats.freed = 0;
    stats.current_usage = 0;
    stats.pool_size = poolSize;
}
