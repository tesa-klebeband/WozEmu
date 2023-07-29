#include "memory.h"

Memory::Memory(Keyboard *kbd, Display *dsp) {
    this->dsp = dsp;
    this->kbd = kbd;
    reset();
}

void Memory::reset() {
    memset(memory, 0, 0xFF00);
    memcpy(memory + 0xFF00, wozmon, 256);
}

uint8_t Memory::read8(uint16_t addr) {
    if (addr == KBD_CTRL) {
        return kbd->read_cr();
    } else if (addr == KBD_DATA) {
        return kbd->read_key();
    }
    return memory[addr];
}

void Memory::write8(uint16_t addr, uint8_t data) {
    if (addr == DSP_DATA) {
        dsp->print(data);
    } else {
        memory[addr] = data;
    }
}

uint16_t Memory::read16(uint16_t addr) {
    return memory[addr] | (memory[addr + 1] << 8);
}

void Memory::write16(uint16_t addr, uint16_t data) {
    memory[addr] = data;
    memory[addr + 1] = data >> 8;
}

void Memory::load(const char *filename, uint16_t addr) {
    std::ifstream image(filename);
    if (!image.is_open())
    {
        fprintf(stderr, "Error: could not open binary\n");
        return;
    }
    image.seekg(0, std::ios::end);
    size_t length = image.tellg();
    image.seekg(0, std::ios::beg);
    image.read((char*) memory + addr, length);

    printf("Loaded %s at 0x%04X successfully!\n", filename, addr);
}