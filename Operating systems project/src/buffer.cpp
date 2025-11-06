//
// Created by os on 9/22/25.
//

#include "../h/buffer.h"

void* Buffer::operator new(size_t size) {
    return MemoryAllocator::kmalloc(size);
}

void Buffer::operator delete(void* ptr) {
    MemoryAllocator::kfree(ptr);
}


void Buffer::putb(char c){
    spaceAvailable.wait();
    buffer[head]=c;
    head=(head+1)%bufferSize;
    itemAvailable.signal();
}

char Buffer::getb(){
    itemAvailable.wait();
    char c=buffer[tail];
    tail=(tail+1)%bufferSize;
    spaceAvailable.signal();
    return c;
}

