//
// Created by os on 9/22/25.
//

#include "../h/Console.h"
#include "../lib/hw.h"

Buffer KConsole::inputBuffer;
Buffer KConsole::outputBuffer;

void KConsole::putc(char c) {
    outputBuffer.putb(c);
}

char KConsole::getc() {
    return inputBuffer.getb();
}

