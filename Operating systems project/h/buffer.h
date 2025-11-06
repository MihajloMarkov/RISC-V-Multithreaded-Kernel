//
// Created by os on 9/22/25.
//

#ifndef OS_PROJEKAT_BUFFER_HPP
#define OS_PROJEKAT_BUFFER_HPP

#include "Semaphore.h"


class Buffer{
public:
    Buffer(): spaceAvailable(bufferSize), itemAvailable(0),head(0),tail(0){};

    void putb(char c);
    char getb();

    bool empty(){return head==tail;}

    void* operator new(size_t size);
    void operator delete(void* ptr);

private:
    Sem spaceAvailable,itemAvailable;
    uint64 head,tail;
    static const uint64 bufferSize = 1024;
    char buffer[bufferSize];
};

#endif //OS_PROJEKAT_BUFFER_HPP
