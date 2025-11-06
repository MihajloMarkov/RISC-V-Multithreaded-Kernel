//
// Created by os on 9/22/25.
//

#ifndef OS_PROJEKAT_CONSOLE_HPP
#define OS_PROJEKAT_CONSOLE_HPP

#include "buffer.h"

class KConsole{
public:

    static void putc(char c);
    static char getc();

    friend class Riscv;
private:
    static Buffer inputBuffer,outputBuffer;
};

#endif //OS_PROJEKAT_CONSOLE_HPP
