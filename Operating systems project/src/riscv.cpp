

#include "../h/riscv.h"
#include "../h/tcb.h"
#include "../lib/console.h"
#include "../h/syscall_c.h"
#include "../h/Semaphore.h"
#include "../h/ThreadSleep.h"
#include "../h/Console.h"

void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}


#define A0 10
static inline void put_on_user_stack(int reg, uint64 value)
{
    asm volatile("sd %0, 8 * %1(fp)" :: "r"(value), "i"(reg));
}

void Riscv::handleSupervisorTrap(uint64 first, uint64 second, uint64 third, uint64 fourth, uint64 fifth, uint64 sixth)
{

    uint64 volatile sepc = r_sepc();
    uint64 volatile sstatus = r_sstatus();
    uint64 scause=r_scause();

    if(scause==8ul || scause == 9ul)
    {
        sepc += 4;
        uint64 opCode = first;
        if(opCode==0x01)//mem_alloc
        {
            size_t size = second;
            size=size*MEM_BLOCK_SIZE;
            put_on_user_stack(A0, (uint64)MemoryAllocator::kmalloc(size));
        }
        else if(opCode==0x02)//mem_free
        {
            void* memptr = (void*)second;
            put_on_user_stack(A0, MemoryAllocator::kfree(memptr));
        }
        else if(opCode==0x03)//total free space
        {
            put_on_user_stack(A0, MemoryAllocator::kTotalFree());
        }
        else if(opCode==0x04)//largest free block
        {
            put_on_user_stack(A0, MemoryAllocator::KLargestFreeBlock());
        }
        else if (opCode == 0x11)// thread_create
        {
            thread_t* handle = (thread_t*)second;
            void(*start_routine)(void*) = (void(*)(void *))third;
            void* arg = (void*)fourth;
            void* stack = (void*)fifth;
            TCB::prio prio = (TCB::prio)sixth;

            *handle = (thread_t)TCB::createThread(start_routine, arg, true, stack,true, prio);

            if (*handle != nullptr)
            {
               put_on_user_stack(A0, (uint64)*handle);
            }
            else
            {
                put_on_user_stack(A0, -1);
            }
        }
        else if (opCode == 0x12)//thread_exit
        {
            TCB::running->setFinished(true);

            TCB::running->parent->numChildren--;
            if(TCB::running->parent->joiner->value() < 0) TCB::running->parent->joiner->signal();
            if(TCB::running->parent->joinAllSem->value() < 0) TCB::running->parent->joinAllSem->signal();
            TCB::running->joinHandleSem->close();
            TCB::maxt->signal();

            TCB::dispatch();
            put_on_user_stack(A0, -1);
        }
        else if (opCode == 0x13)//thread_dispatch
        {
            TCB::dispatch();
        }
        else if(opCode == 0x14)//getThreadId
        {
            put_on_user_stack(A0,TCB::running->getId());
            TCB::dispatch();
        }
        else if(opCode==0x21)//sem_open
        {
            sem_t* handle = (sem_t*)second;
            unsigned init = third;

            *handle=(sem_t)Sem::createSemaphore(init);

            if (*handle != nullptr)
            {
                put_on_user_stack(A0, 0);
            }
            else
            {
                put_on_user_stack(A0, 1);
            }
        }
        else if(opCode==0x22)//sem_close
        {
            sem_t handle = (sem_t)second;
            if(handle== nullptr)put_on_user_stack(A0, -1);
            else
            {
                Sem* sem = (Sem*)handle;
                put_on_user_stack(A0, sem->close());
            }
        }
        else if(opCode==0x23)//sem_wait
        {
            sem_t handle = (sem_t)second;
            if(handle== nullptr)put_on_user_stack(A0, -1);
            else
            {
                Sem* sem = (Sem*)handle;
                put_on_user_stack(A0, sem->wait());
            }
        }
        else if(opCode==0x24)//sem_signal
        {
            sem_t handle = (sem_t)second;
            if(handle== nullptr)put_on_user_stack(A0, -1);
            else
            {
                Sem* sem = (Sem*)handle;
                put_on_user_stack(A0, sem->signal());
            }
        }
        else if (opCode==0x31)//time_sleep
        {
            time_t time = second;
            put_on_user_stack(A0, SleepManager::threadSleep(time));
        }
        else if(opCode==0x41)//getc
        {
            char c = __getc();
            put_on_user_stack(A0, c);
        }
        else if(opCode==0x42)//putc
        {
            char c = second;
            __putc(c);
        }
        else if (opCode == 0x50 ) {
            uint64 num = second;
            TCB::setMaxThreads((int)num);
        }
        else if (opCode == 0x60) {
            TCB::running->join();
        }
        else if(opCode == 0x70)
        {
            TCB::running->joinall();
        }
        else if (opCode == 0x80) {
            TCB::joinHandle((TCB*)second);
        }
        else if (opCode == 0x81) { // thread_pair
            TCB* firstT = (TCB*)second;
            TCB* secondT = (TCB*)third;
            firstT->paired = secondT;
            secondT->paired = firstT;
        } else if (opCode == 0x82) { // thread_sync
            if (TCB::running->paired)
            {
                TCB::running->paired->pairSem->signal();
                TCB::running->pairSem->wait();
            }
        } else if (opCode == 0x83) { // msg_send
            TCB* recvT = (TCB*)second;
            char* msg = (char*)third;
            if (!recvT->waitingForMsg) {
                recvT->sender = TCB::running;

                for (int i = 0;msg[i] != 0 && i < 512;i++) {
                    TCB::running->sendBuff[i] = msg[i];

                }
                TCB::running->block();
                TCB::dispatch();
            }
            else {

                for (int i = 0;msg[i] != 0 && i < 512;i++) {
                    recvT->recvBuff[i] = msg[i];

                }
                recvT->unblock();
                recvT->waitingForMsg = false;
                Scheduler::put(recvT);
            }
        } else if (opCode == 0x84) {
            char* destBuff = (char*) second;
            if (TCB::running->sender)
            {
                TCB* sender = TCB::running->sender;
                char * buff = sender->sendBuff;
                for (int i = 0; i < 512 && buff[i]; i++)
                {
                    destBuff[i] = buff[i];
                }

                TCB::running->sender = nullptr;
                sender->unblock();
                Scheduler::put(sender);
            }
            else
            {
                TCB::running->waitingForMsg = true;
                TCB::running->recvBuff = destBuff;
                TCB::running->block();
                TCB::dispatch();
            }
        }

    }
    else if(scause==0x8000000000000001UL)//timer
    {
        handleTimerInterrupt();
    }
    else if(scause==0x8000000000000009UL)//console
    {
        handleConsoleInterrupt();
    }
    else if (scause == 0x0000000000000002UL) // illegal instruction
    {
        __putc('?');
    }
    else if (scause == 0x0000000000000005UL) { // bad read address
        __putc('+');
    }
    else if (scause == 0x0000000000000007UL) { // bad write address
        __putc('*');
    }

    w_sepc(sepc);
    w_sstatus(sstatus);
}

void Riscv::handleTimerInterrupt()
{
    mc_sip(SIP_SSIP);
    // TCB::timeSliceCounter++;
    // SleepManager::wakeUp();
    // uint64 volatile sepc =r_sepc();
    // uint64 volatile sstatus=r_sstatus();
    // if((TCB::timeSliceCounter>=TCB::running->getTimeSlice())) {
    //     TCB::timeSliceCounter=0;
    //     TCB::dispatch();
    // }
    // w_sstatus(sstatus);
    // w_sepc(sepc);
}

void Riscv::handleConsoleInterrupt()
{
    // int irq = plic_claim();
    // if (irq == CONSOLE_IRQ)
    // {
    // /*char status = *(char*)CONSOLE_STATUS;

    // while (status & CONSOLE_RX_STATUS_BIT) {
    //     char c = *(char*)CONSOLE_RX_DATA;
    //     KConsole::inputBuffer.put(c);
    //     status = *(char*)CONSOLE_STATUS;
    // }

    // while ((status & CONSOLE_TX_STATUS_BIT) && !KConsole::outputBuffer.empty()) {
    //     char c = KConsole::outputBuffer.get();
    //     *(char*)CONSOLE_TX_DATA = c;
    //     status = *(char*)CONSOLE_STATUS;
    // }*/
    // }
    // plic_complete(irq);

   console_handler();

}

void Riscv::enter_user_mode() {
    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}