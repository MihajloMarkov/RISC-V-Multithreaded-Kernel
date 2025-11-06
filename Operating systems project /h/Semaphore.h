//
// Created by os on 9/21/25.
//

#ifndef OS_PROJEKAT_SEMAPHORE_H
#define OS_PROJEKAT_SEMAPHORE_H

#include "list.h"
#include "tcb.h"

class Sem {
public:
    Sem (uint64 init=1) : val(init),closed(false) {}
    static Sem* createSemaphore(uint64 init = 1);
    int wait ();
    int signal ();
    int close();
    int value () const { return val; }

    void* operator new(size_t size);
    void operator delete(void* ptr);

private:
    int val;
    bool closed;
    List<TCB> blocked;
};

#endif //OS_PROJEKAT_SEMAPHORE_H
