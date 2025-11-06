//
// Created by os on 9/4/25.
//

#ifndef OS_PROJEKAT_MEMORYALLOCATOR_H
#define OS_PROJEKAT_MEMORYALLOCATOR_H
#include "../lib/hw.h"
class MemoryAllocator
{
public:
    static void init();

    static void* kmalloc(size_t size);
    static int kfree(void* freeAdr);
    static size_t kTotalFree();
    static size_t KLargestFreeBlock();
private:

    struct FreeBlock{
        FreeBlock* next;
        size_t size;
    };
    static FreeBlock* head;

    static void merge(FreeBlock *block1, FreeBlock *block2);

    static bool adjacent(FreeBlock *block1, FreeBlock *block2);
};
#endif //OS_PROJEKAT_MEMORYALLOCATOR_H
