//
// Created by os on 9/12/25.
//

#ifndef OS_PROJEKAT_SYSCALL_C_H
#define OS_PROJEKAT_SYSCALL_C_H

#include "../lib/hw.h"
#include "tcb.h"

void* mem_alloc (size_t size);

int mem_free (void*);

size_t mem_get_free_space();

size_t mem_get_largest_free_block();

class TCB;
typedef TCB* thread_t;

enum thread_prio{
    LOW, MED, HIGH
};

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg, thread_prio prio = LOW);

int thread_exit();

void thread_dispatch();

int getThreadId();

class Sem;
typedef Sem* sem_t;
int sem_open (sem_t* handle,unsigned init);

int sem_close (sem_t handle);

int sem_wait (sem_t id);

int sem_signal (sem_t id);

typedef unsigned long time_t;
int time_sleep (time_t);

const int EOF = -1;
char getc ();

void putc (char);

void join();
void joinall();
void joinHandle(thread_t handle);

void thread_pair(thread_t handle1, thread_t handle2);
void thread_sync();

void setMaximumThreads(int num) ;

#endif //OS_PROJEKAT_SYSCALL_C_H
