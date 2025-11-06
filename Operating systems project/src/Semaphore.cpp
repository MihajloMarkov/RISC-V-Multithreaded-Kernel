//
// Created by os on 9/21/25.
//
#include "../h/Semaphore.h"
#include "../h/tcb.h"


void* Sem::operator new(size_t size) {
    return MemoryAllocator::kmalloc(size);
}

void Sem::operator delete(void* ptr) {
    MemoryAllocator::kfree(ptr);
}

int Sem::wait() {
    val--;
    if(val<0) {
        TCB *runningThread = TCB::running;
        blocked.addLast(runningThread);
        runningThread->block();
        TCB::dispatch();
    }
    if(this->closed)return -1;
    return 0;
}

int Sem::signal() {
    if(this->closed)return -1;
    val++;
    if(val<=0)
    {
        TCB* unblockedThread = blocked.removeFirst();
        unblockedThread->unblock();
        Scheduler::put(unblockedThread);
    }
    return 0;
}

int Sem::close() {
    if(closed)return -1;
    closed=true;
    while(blocked.peekFirst())
    {
        TCB* cur=blocked.removeFirst();
        cur->unblock();
        Scheduler::put(cur);
    }
    return 0;
}

Sem *Sem::createSemaphore(uint64 init) {
    return new Sem(init);
}
