
#ifndef OS_PROJEKAT_SCHEDULER_H
#define OS_PROJEKAT_SCHEDULER_H

#include "list.h"

class TCB;

class Scheduler
{
public:
    static List<TCB> readyThreadQueue1;
    static List<TCB> readyThreadQueue2;
    static List<TCB> readyThreadQueue3;

public:
    static TCB *get();

    static void put(TCB *ccb);

};


#endif