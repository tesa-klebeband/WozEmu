#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdio>

#define DSP_DATA 0xD012
#define DSP_CTRL 0xD013

class Display {
    public:
        void print(char c);
};

#endif