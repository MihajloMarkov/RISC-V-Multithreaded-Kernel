//
// Created by os on 9/11/25.
//


#ifndef OS_PROJEKAT_CCB_H
#define OS_PROJEKAT_CCB_H

#include "../lib/hw.h"
#include "Scheduler.h"
#include "MemoryAllocator.h"

//#include "Semaphore.h"

class Sem;

// Thread Control Block
class TCB
{
public:

    enum class prio{
        LOW, MED, HIGH
    };
    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    void block(){blocked=true;}

    void unblock(){blocked=false;}

    bool isBlocked(){return blocked;}

    uint64 getTimeSlice() const{return timeSlice;}

    uint64 getId() const{return id;}

    using Body = void (*)(void*);

    static TCB *createThread(Body body, void*arg, bool put_into_scheduler, void* stack,bool userThread, prio priority);

    static void yield();

    static TCB *running;

    void* operator new(size_t size);
    void operator delete(void* ptr);

    static void setMaxThreads(int num = 1000);

    prio getPriority() {return priority; }

     bool operator<(const TCB& t2) const
    {
        return id < t2.id;
    }
private:
    static Sem* maxt;

    Sem* joiner;
    Sem* joinAllSem;
    Sem* joinHandleSem;


    TCB(Body body,uint64 timeSlice, void* arg, bool put_into_scheduler, void* stack, bool userThread, prio priority = prio::LOW) :
            body(body),
           stack((char*)stack),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &(((char*)stack)[DEFAULT_STACK_SIZE]) : 0
                    }),
            finished(false),
            timeSlice(timeSlice),
            arg(arg),
            id(ID_COUNTER++),
            parent(TCB::running),
            blocked(false),
            sleeping(false),
            sleepTime(-1),
            userThread(userThread),
            numChildren(0),
            waitingFor(nullptr),
            priority(priority)
            {
                if(body != nullptr && put_into_scheduler){Scheduler::put(this);}
                createJoiner();
            }


    struct Context
    {
        uint64 ra;
        uint64 sp;
    };
    Body body;
    char *stack;
    Context context;
    bool finished;
    uint64  timeSlice;
    void* arg;

    static uint64 ID_COUNTER;
    const uint64 id;
    TCB* parent;

    bool blocked;

    bool sleeping;
    uint64 sleepTime;

    static uint64 maxThreads;
    static uint64 threadCount;
    bool userThread;

    friend class Riscv;
    friend class SleepManager;
    friend class Sem;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void  threadWrapper();

    static void dispatch();

    static  uint64  timeSliceCounter;


    void createJoiner();

    void join();
    void joinall();
    void static joinHandle(TCB* handle);

    int numChildren;
    TCB* waitingFor;

    TCB* paired = nullptr;
    Sem* pairSem;
    prio priority;


    char sendBuff[512], *recvBuff;
    TCB* sender = nullptr;
    bool waitingForMsg = false;


};

#endif