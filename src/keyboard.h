#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#define KBD_DATA 0xD010
#define KBD_CTRL 0xD011

class Keyboard {
    public:
        Keyboard();
        char read_key();
        char read_cr();
};

#endif