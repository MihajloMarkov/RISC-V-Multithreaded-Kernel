//
// Created by os on 10/29/25.
//

#include "CustomTest.h"
#include "printing.hpp"
#include "../h/syscall_cpp.h"


void idleWorker(void* nul)
{
    printString("Idem");
    while(getc() != 0x1b) thread_dispatch();
    printString("Odoh");
}

void waiterWorker(void* otherThread)
{
    Thread* t = (Thread*) otherThread;

    printString("Cekam");
    t->joinHandle();
    printString("Sacekao");
    for(int i = 0; i < 10; i++)Thread::dispatch();
    Thread::sync();
    printString("Poveo");
}

void followerWorker(void* nul)
{
    printString("Dolazim");
    Thread::sync();
    printString("Ispratio");
}

void customTest()
{
    Thread t1(idleWorker, nullptr);
    Thread t2(waiterWorker, &t1);
    Thread t3(followerWorker, nullptr);

    t1.start();
    t2.start();
    t3.start();
    Thread::pair(&t2, &t3);
    t1.joinHandle();
    t2.joinHandle();
    t3.joinHandle();
}