#include "keyboard.h"

Keyboard::Keyboard() {
    struct termios attr;
    tcgetattr(STDIN_FILENO, &attr);
    attr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &attr);
}

char Keyboard::read_key() {
    char kbd_data;
    read(STDIN_FILENO, &kbd_data, 1);
    if (kbd_data >= 'a' && kbd_data <= 'z') {
        kbd_data = kbd_data - 'a' + 'A';
    }
    if (kbd_data == '\n') {
        kbd_data = 0xD;
    }
    return 0x80 | kbd_data;
}

char Keyboard::read_cr() {
    int waiting_bytes;
    ioctl(STDIN_FILENO, FIONREAD, &waiting_bytes);
    if (waiting_bytes) {
        return 0x80;
    } else {
        return 0;
    }
}