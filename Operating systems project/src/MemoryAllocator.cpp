#include "../h/MemoryAllocator.h"
#include "../lib/hw.h"

MemoryAllocator::FreeBlock *MemoryAllocator::head = nullptr;
static const size_t HEADER_SIZE = MEM_BLOCK_SIZE;
void MemoryAllocator::init() {
    head=(FreeBlock*)(((uint64)HEAP_START_ADDR/MEM_BLOCK_SIZE+1)*MEM_BLOCK_SIZE);
    head->next= nullptr;
    head->size=(((uint64)HEAP_END_ADDR/MEM_BLOCK_SIZE-1)*(MEM_BLOCK_SIZE))-(((uint64)HEAP_START_ADDR/MEM_BLOCK_SIZE+1)*MEM_BLOCK_SIZE)-HEADER_SIZE;
}

void *MemoryAllocator::kmalloc(size_t size)
{
    FreeBlock* cur=head;
    FreeBlock* prev= nullptr;
    FreeBlock* nextfree= nullptr;
    while(cur && cur->size<size)
    {
        prev=cur;
        cur=cur->next;
    }
    if(!cur)return nullptr;
    if(cur->size-size>HEADER_SIZE)
    {
        nextfree=(FreeBlock*)((uint64)cur+size+HEADER_SIZE);
        nextfree->size=cur->size-size-HEADER_SIZE;
        nextfree->next=cur->next;
        cur->size=size;
    }
    else
    {
        nextfree=cur->next;
    }
    cur->next=(FreeBlock*)(cur->size^(uint64)cur);
    if(prev)prev->next=nextfree;
    else head=nextfree;
    return (void*)((uint64)cur+HEADER_SIZE);
}

int MemoryAllocator::kfree(void *freeAdr)
{
    FreeBlock* header=(FreeBlock*)((uint64)freeAdr-HEADER_SIZE);
    if(header<HEAP_START_ADDR)return -1;
    if (header >= (FreeBlock*)HEAP_END_ADDR) return -1;
    if(header->next!=(FreeBlock*)((uint64)header^header->size))return -2;
    FreeBlock* prev=head;
    FreeBlock* nxt= head->next;
    if(header<head)
    {
        nxt=head;
        head=header;
        head->next = nxt;
        prev= nullptr;
    }
    else
    {
        while(nxt && header>nxt)
        {
            prev=nxt;
            nxt=nxt->next;
        }
        if (prev) prev->next = nxt;
        else head = nxt;
    }
    merge(header,nxt);
    merge(prev,header);
    return 0;
}

void MemoryAllocator::merge(FreeBlock *block1, FreeBlock *block2)
{
    if(!block1)return;
    if(!block2)
    {
        block1->next= nullptr;
        return;
    }
    if(adjacent(block1,block2)) {
        block1->size += block2->size + HEADER_SIZE;
        block1->next = block2->next;
    }
}

bool MemoryAllocator::adjacent(MemoryAllocator::FreeBlock *block1, MemoryAllocator::FreeBlock *block2) {
    return (uint64)block1+block1->size+HEADER_SIZE==(uint64)block2;
}

size_t MemoryAllocator::kTotalFree() {
    size_t ans=0;
    FreeBlock* cur=head;
    while (cur)
    {
        ans+=cur->size;
        cur=cur->next;
    }
    return ans;
}

size_t MemoryAllocator::KLargestFreeBlock() {
    size_t ans=0;
    FreeBlock* cur=head;
    while(cur)
    {
        if(cur->size>ans)ans=cur->size;
        cur=cur->next;
    }
    return ans;
}

