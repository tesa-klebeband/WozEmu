#include "display.h"

void Display::print(char c) {
    if (c & 0x80) {
        c &= 0x7F;
        if (c == 0xD) c = '\n';
        fprintf(stderr, "%c", c & 0x7F);
    }
}