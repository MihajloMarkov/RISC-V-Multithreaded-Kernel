#include "../h/tcb.h"
#include "../h/riscv.h"
#include "../h/Scheduler.h"
#include "../h/syscall_cpp.h"
#include "../lib/hw.h"
#include "../lib/console.h"
#include "../h/Semaphore.h"

TCB *TCB::running = nullptr;
uint64 TCB::timeSliceCounter=0;
uint64 TCB::ID_COUNTER=0;
uint64 TCB::maxThreads=1000;
uint64 TCB::threadCount=0;
Sem* TCB::maxt = nullptr;

void* TCB::operator new(size_t size) {
    return MemoryAllocator::kmalloc(size);
}

void TCB::operator delete(void* ptr) {
    MemoryAllocator::kfree(ptr);
}

TCB *TCB::createThread(TCB::Body body, void* arg, bool put_into_scheduler, void* stack,bool userThread, TCB::prio priority){
    if(TCB::running) TCB::running->numChildren++;
    return new TCB(body,DEFAULT_TIME_SLICE,arg, put_into_scheduler, stack,userThread, priority);
}

void TCB::yield()
{
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

void TCB::dispatch() {
    TCB *old=running;
    if(!old->isFinished() && !old->isBlocked()){
        Scheduler::put(old);
    }
    running=Scheduler::get();

    TCB::contextSwitch(&old->context,&running->context);
}

void TCB::threadWrapper()
{
    maxt->wait();
    Riscv::enter_user_mode();


    running->body(running->arg);


    thread_exit();
}

void TCB::setMaxThreads(int num) {
    maxThreads = num;
    maxt = new Sem(num);
}

void TCB::createJoiner() {
    joiner = new Sem(0);
    joinAllSem= new Sem(0);
    joinHandleSem=new Sem(0);
    pairSem = new Sem(0);
}

void TCB::join() {
    joiner->wait();
}

void TCB::joinall() {
    while (numChildren > 0) {
        joinAllSem->wait();
    }
}

void TCB::joinHandle(TCB* handle)
{
    if (!handle->isFinished())
        handle->joinHandleSem->wait();
}
