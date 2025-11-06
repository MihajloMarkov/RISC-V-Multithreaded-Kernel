#include "../h/syscall_c.h"
#include "../lib/console.h"


void* mem_alloc (size_t size)
{
    size=(size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    __asm__ volatile("mv a1, %0" : : "r"(size));
    __asm__ volatile("li a0, 0x01");
    __asm__ volatile("ecall");

    void* returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

int mem_free (void* ptr)
{
    __asm__ volatile("mv a1, %0" : : "r"(ptr));
    __asm__ volatile("li a0, 0x02");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

size_t mem_get_free_space()
{
    __asm__ volatile("li a0, 0x03");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

size_t mem_get_largest_free_block()
{
    __asm__ volatile("li a0, 0x04");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg, thread_prio prio)
{
    void* stack = mem_alloc(DEFAULT_STACK_SIZE);

    __asm__ volatile("mv a5, %0" : : "r"(prio));
    __asm__ volatile("mv a4, %0" : : "r"(stack));
    __asm__ volatile("mv a3, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"(start_routine));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x11");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

int thread_exit()
{
    __asm__ volatile("li a0, 0x12");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

void thread_dispatch()
{
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

int getThreadId()
{
    __asm__ volatile("li a0, 0x14");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

int sem_open (sem_t* handle,unsigned init)
{
    __asm__ volatile("mv a2, %0" : : "r"(init));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x21");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}


int sem_close (sem_t handle)
{
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x22");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}


int sem_wait (sem_t id)
{
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x23");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}


int sem_signal (sem_t id)
{
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x24");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

int time_sleep (time_t time)
{
    __asm__ volatile("mv a1, %0" : : "r"(time));
    __asm__ volatile("li a0, 0x31");
    __asm__ volatile("ecall");

    int returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

char getc()
{
    __asm__ volatile("li a0, 0x41");
    __asm__ volatile("ecall");

    char returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

void putc(char c)
{
    __asm__ volatile("mv a1, %0" : : "r"(c));
    __asm__ volatile("li a0, 0x42");
    __asm__ volatile("ecall");
}

void setMaximumThreads(int num) {
    __asm__ volatile("mv a1, %0" : : "r"(num));
    __asm__ volatile("li a0, 0x50");
    __asm__ volatile("ecall");
}

void join() {
    __asm__ volatile("li a0, 0x60");
    __asm__ volatile("ecall");
}
void joinall(){
    __asm__ volatile("li a0, 0x70");
    __asm__ volatile("ecall");
}

void joinHandle(thread_t handle) {
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x80");
    __asm__ volatile("ecall");
}

void thread_pair(thread_t handle1, thread_t handle2) {
    __asm__ volatile("mv a2, %0" : : "r"(handle2));
    __asm__ volatile("mv a1, %0" : : "r"(handle1));
    __asm__ volatile("li a0, 0x81");
    __asm__ volatile("ecall");
}

void thread_sync(){
    __asm__ volatile("li a0, 0x82");
    __asm__ volatile("ecall");
}