//
// Created by os on 9/21/25.
//

#include "../h/syscall_cpp.h"
#include "../h/MemoryAllocator.h"

void* operator new (size_t size){
    return mem_alloc(size);
}

void operator delete (void* mem){
    mem_free(mem);
}

void *operator new[](size_t size) {
    return mem_alloc(size);
}

void operator delete[](void *mem) {
    mem_free(mem);
}

Thread::Thread(void (*body)(void *), void *arg, thread_prio prio):body(body), arg(arg), prio(prio) { }

Thread::~Thread() = default;

int Thread::start(){
    if (myHandle == nullptr) return thread_create(&myHandle, body, arg, prio);
    return 0;
}

void Thread::dispatch(){
    thread_dispatch();
}

int Thread::getId() {
    return getThreadId();
}

int Thread::sleep(time_t time){
    return time_sleep(time);
}

void Thread::threadWrapper(void* ptr)
{
    Thread* thisthread = (Thread*) ptr;
    thisthread->run();
}

void Thread::joinHandle() {
    ::joinHandle(myHandle);
}

Thread::Thread(thread_prio prio): body(threadWrapper), arg(this), prio(prio) {}

void Thread::pair(Thread *first, Thread *second) {
    thread_pair(first->myHandle, second->myHandle);
}

void Thread::sync()
{
    thread_sync();
}


Semaphore::Semaphore (unsigned init){
    sem_open(&myHandle,init);
}

Semaphore::~Semaphore (){
    sem_close(myHandle);
}

int Semaphore::wait (){
    return sem_wait(myHandle);
}

int Semaphore::signal (){
    return sem_signal(myHandle);
}


void PeriodicThread::terminate (){
    this->period=0;
}

PeriodicThread::PeriodicThread (time_t period){
    this->period=period;
}

void PeriodicThread::run()
{
    while (period != 0)
    {
        Thread::sleep(period);
        periodicActivation();
    }
}

void Console::putc(char c) {
    ::putc(c);
}

char Console::getc() {
    return ::getc();
}
