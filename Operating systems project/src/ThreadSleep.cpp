//
// Created by os on 9/21/25.
//
#include "../h/ThreadSleep.h"
#include "../h/Scheduler.h"
#include "../h/syscall_c.h"

uint64 SleepManager::curtime=0;
SleepManager::SleepingThread* SleepManager::head = nullptr;

void* SleepManager::SleepingThread::operator new(size_t size) {
    return MemoryAllocator::kmalloc(size);
}

void SleepManager::SleepingThread::operator delete(void* ptr) {
    MemoryAllocator::kfree(ptr);
}

int SleepManager::threadSleep(uint64 time) {
    if(time==0)return 0;
    TCB* running=TCB::running;
    running->sleeping= true;
    running->sleepTime=time;
    insertThread(running,time);
    TCB::dispatch();
    return 0;
}

void SleepManager::wakeUp() {
    if(head==nullptr) return;
    head->relativeSleepTime--;
    while(head && head->relativeSleepTime==0)
    {
        head->thread->sleeping=false;
        head->thread->sleepTime=-1;
        Scheduler::put(head->thread);
        SleepingThread* oldhead=head;
        head=head->next;
        delete oldhead;
    }
}

void SleepManager::insertThread(TCB* thread, uint64 time){
    SleepingThread* newSleepingThread=new SleepingThread();
    newSleepingThread->thread=thread;
    newSleepingThread->relativeSleepTime=time;
    newSleepingThread->next= nullptr;
    if(head==nullptr)
    {
        head=newSleepingThread;
        return;
    }
    if(newSleepingThread->relativeSleepTime<head->relativeSleepTime)
    {
        newSleepingThread->next=head;
        head->relativeSleepTime-=newSleepingThread->relativeSleepTime;
        head=newSleepingThread;
        return;
    }
    SleepingThread* cur=head;
    SleepingThread* prev= nullptr;
    while(cur && cur->relativeSleepTime>=newSleepingThread->relativeSleepTime)
    {
        newSleepingThread->relativeSleepTime-=cur->relativeSleepTime;
        prev=cur;
        cur=cur->next;
    }
    newSleepingThread->next=cur;
    prev->next=newSleepingThread;
    if(cur)cur->relativeSleepTime-=newSleepingThread->relativeSleepTime;
}