#include "wozemu.h"

Apple1 *apple1;
struct termios attr;

void wozemu_init() {
    tcgetattr(STDIN_FILENO, &attr);
    signal(SIGINT, wozemu_interface);

    apple1 = new Apple1;
}

void wozemu_run() {
    while(1) {
        apple1->cycle();
        usleep(1);
    }
}

void wozemu_interface(int signal) {
    struct termios old_attr;
    tcgetattr(STDIN_FILENO, &old_attr);
    tcsetattr(STDIN_FILENO, TCSANOW, &attr);

    printf("\n=== WozEmu menu ===\n");
    printf("Key bindings: \n");
    printf("* Quit:         q\n");
    printf("* Exit menu:    x\n");
    printf("* Reset:        r\n");
    printf("* Power cycle:  p\n");
    printf("* Load binary:  l\n");
    printf("* Clear screen: c\n");
    printf("\nEnter choice: ");

    char in;
    std::cin >> in;

    switch (in) {
        case 'q': delete apple1; exit(0);
        case 'x': break;
        case 'r': apple1->reset(); break;
        case 'p': apple1->power_cycle(); break;
        case 'l': {
            std::string filename;
            uint16_t addr;

            printf("Enter filename: ");
            std::cin >> filename;
            printf("Enter load address (hex): ");
            std::cin >> std::hex >> addr;
            apple1->load(filename.c_str(), addr);
            break;
        }
        case 'c': system("clear"); break;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
}

Apple1::Apple1() : memory(&kbd, &dsp), cpu(&memory) {
}

void Apple1::cycle() {
    cpu.cycle();
}

void Apple1::reset() {
    cpu.reset();
}

void Apple1::power_cycle() {
    memory.reset();
    cpu.reset();
}

void Apple1::load(const char *filename, uint16_t addr) {
    memory.load(filename, addr);
}