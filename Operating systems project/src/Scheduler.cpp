//
// Created by os on 9/17/25.
//

#include "../h/Scheduler.h"
#include "../h/tcb.h"

List<TCB> Scheduler::readyThreadQueue1;
List<TCB> Scheduler::readyThreadQueue2;
List<TCB> Scheduler::readyThreadQueue3;

TCB* Scheduler::get()
{
    if (readyThreadQueue1.peekFirst())
        return readyThreadQueue1.removeFirst();
    if (readyThreadQueue2.peekFirst())
        return readyThreadQueue2.removeFirst();

    return readyThreadQueue3.removeFirst();
}

void Scheduler::put(TCB* tcb)
{
    switch (tcb->getPriority())
    {
        case TCB::prio::LOW:
            readyThreadQueue3.addLast(tcb);
            break;
        case TCB::prio::MED:
            readyThreadQueue2.addLast(tcb);
            break;
        case TCB::prio::HIGH:
            readyThreadQueue1.addLast(tcb);
            break;
        default:
            readyThreadQueue1.addLast(tcb);
    }
}