#ifndef WOZEMU_H
#define WOZEMU_H

#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <iostream>

#include "cpu.h"
#include "memory.h"
#include "display.h"
#include "keyboard.h"

void wozemu_init();
void wozemu_run();
void wozemu_interface(int signal);

class Apple1 {
    public:
        Apple1();
        void cycle();
        void reset();
        void power_cycle();
        void load(const char *filename, uint16_t addr);

    private:
        Keyboard kbd;
        Display dsp;
        Memory memory;
        MOS6502 cpu;

};

#endif