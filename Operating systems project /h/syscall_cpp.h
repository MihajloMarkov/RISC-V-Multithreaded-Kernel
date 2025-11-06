//
// Created by os on 9/21/25.
//

#ifndef _syscall_cpp
#define _syscall_cpp
#include "syscall_c.h"

void* operator new (size_t);
void operator delete (void*);

class Thread {
public:
    Thread (void (*body)(void*), void* arg, thread_prio prio = LOW);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static  int getId();
    static int sleep (time_t);
    static void pair(Thread* first, Thread* second);
    static void sync();
    void joinHandle();
protected:
    explicit Thread (thread_prio prio = LOW);
    virtual void run () {}
private:
    thread_t myHandle = nullptr;
    void (*body)(void*); void* arg; thread_prio prio;
    static void threadWrapper(void* ptr);
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
public:
    void terminate ();
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
    void run()override;
private:
    time_t period;
};

class Console {
public:
    static char getc ();
    static void putc (char);
};
#endif