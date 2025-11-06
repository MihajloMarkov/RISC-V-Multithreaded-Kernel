#include "../lib/hw.h"
#include "../lib/console.h"
#include "../h/riscv.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_c.h"
#include "../h/tcb.h"
void userMain();

sem_t userMainSemaphore;

void userMainWrapper(void* arg) {
    userMain();
    sem_signal(userMainSemaphore);
}

void idleThread(void*) {
    while (true) {
        thread_dispatch();
    }
}
int main() {
    Riscv::w_stvec((uint64)(&Riscv::supervisorTrap));
    
    MemoryAllocator::init();

    TCB::setMaxThreads();
    TCB* current = TCB::createThread(nullptr, nullptr, false, nullptr, true, TCB::prio::LOW);
    TCB::running = current;    

    sem_open(&userMainSemaphore, 0);
    thread_t user;

    thread_create(&user, userMainWrapper, nullptr);

    sem_wait(userMainSemaphore);
   
    return 0;
}