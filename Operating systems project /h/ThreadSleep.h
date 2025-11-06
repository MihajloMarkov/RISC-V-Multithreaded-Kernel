//
// Created by os on 9/21/25.
//

#ifndef OS_PROJEKAT_THREADSLEEP_H
#define OS_PROJEKAT_THREADSLEEP_H

#include "list.h"
#include "tcb.h"

class SleepManager
{
public:
    static int threadSleep(uint64 time);
private:

    friend class Riscv;

    struct SleepingThread{
        TCB* thread;
        uint64 relativeSleepTime;
        SleepingThread* next;

        void* operator new(size_t size);
        void operator delete(void* ptr);
    };
    static void wakeUp();
    static void insertThread(TCB* thread, uint64 time);
    static uint64 curtime;
    static SleepingThread* head;
};

#endif //OS_PROJEKAT_THREADSLEEP_H
