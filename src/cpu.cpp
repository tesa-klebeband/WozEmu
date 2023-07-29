#include "cpu.h"

#define BIN_TO_BCD(byte) (((byte) / 10) << 4) | ((byte) % 10)
#define BCD_TO_BIN(bcd) (((bcd) >> 4) * 10) + ((bcd) & 0x0F)

MOS6502::MOS6502(Memory *memory) {
    this->memory = memory;
    reset();
}

void MOS6502::reset() {
    a = x = y = 0;
    sp = 0xFF;
    p = 0x20;
    pc = memory->read16(0xFFFC);
}

void MOS6502::cycle() {
    uint8_t opcode = memory->read8(pc);
    uint8_t a_op = opcode >> 5;
    uint8_t b_op = (opcode >> 2) & 0b111;
    uint8_t c_op = opcode & 0b11;

    switch (c_op) {
        case 0: {
            switch (b_op) {
                case 0: {
                    switch (a_op) {
                        case 0: brk_op(); break;
                        case 1: jsr_op(); break;
                        case 2: rti_op(); break;
                        case 3: rts_op(); break;
                        case 5: load_op(2, &y); break;
                        case 6: cmp_op(2, &y); break;
                        case 7: cmp_op(2, &x); break;
                    }
                    break;
                }
                case 1: {
                    switch(a_op) {
                        case 1: bit_op(b_op); break;
                        case 4: store_op(b_op, &y); break;
                        case 5: load_op(b_op, &y); break;
                        case 6: cmp_op(b_op, &y); break;
                        case 7: cmp_op(b_op, &x); break;
                    }
                    break;
                }
                case 2: {
                    switch (a_op) {
                        case 0: php_op(); break;
                        case 1: plp_op(); break;
                        case 2: pha_op(); break;
                        case 3: pla_op(); break;
                        case 4: der_op(&y); break;
                        case 5: transf_op(&a, &y); break;
                        case 6: inr_op(&y); break;
                        case 7: inr_op(&x); break;
                    }
                    break;
                }
                case 3: {
                    switch (a_op) {
                        case 1: bit_op(b_op); break;
                        case 2: jmp_op(b_op); break;
                        case 3: jmp_op(0xFF); break;
                        case 4: store_op(b_op, &y); break;
                        case 5: load_op(b_op, &y); break;
                        case 6: cmp_op(b_op, &y); break;
                        case 7: cmp_op(b_op, &x); break;
                    }
                    break;
                }
                case 4: {
                    branch_op(a_op);
                    break;
                }
                case 5: {
                    if (a_op == 4) {
                        store_op(b_op, &y);
                    } else if (a == 5) {
                        load_op(b_op, &y);
                    }
                    break;
                }
                case 6: {
                    if (a_op != 4) {
                        mod_flags_op(a_op);
                    } else {
                        transf_op(&y, &a);
                    }
                    break;
                }
                case 7: {
                    if (a_op == 5) {
                        load_op(b_op, &y);
                    }
                    break;
                }
            }
            break;
        }
        case 1: {
            switch (a_op) {
                case 3: adc_op(b_op); break;
                case 4: store_op(b_op, &a); break;
                case 5: load_op(b_op, &a); break;
                case 6: cmp_op(b_op, &a); break;
                case 7: sbc_op(b_op); break;
                default: bitwise_op(b_op, a_op); break;
            }
            break;
        }
        case 2: {
            switch (a_op) {
                case 0: asl_op(b_op); break;
                case 1: rol_op(b_op); break;
                case 2: lsr_op(b_op); break;
                case 3: ror_op(b_op); break;
                case 4: {
                    switch (b_op) {
                        case 1: store_op(b_op, &x); break;
                        case 2: transf_op(&x, &a); break;
                        case 3: store_op(b_op, &x); break;
                        case 5: store_op(8, &x); break;
                        case 6: transf_op(&x, &sp); break;
                    }
                    break;
                }
                case 5: {
                    switch (b_op) {
                        case 0: load_op(2, &x); break;
                        case 1: load_op(b_op, &x); break;
                        case 2: transf_op(&a, &x); break;
                        case 3: load_op(b_op, &x); break;
                        case 5: load_op(8, &x); break;
                        case 6: transf_op(&sp, &x); break;
                        case 7: load_op(6, &x); break;
                    }
                    break;
                }
                case 6: {
                    if (b_op == 2) {
                        der_op(&x);
                    } else {
                        dec_op(b_op);
                    }
                    break;
                }
                case 7: {
                    if (b_op != 2) {
                        inc_op(b_op);
                    }
                    break;
                }
            }
            break;
        }
        break;
    }
    pc++;
}

void MOS6502::bitwise_op(uint8_t mode, uint8_t op) {
    uint8_t data = op_mode_read(mode);
    if (op == 0) {
        a |= data;
    } else if (op == 1) {
        a &= data;
    } else {
        a ^= data;
    }
    p = (p & 0x7F) | (a & 0x80);
    p = (p & 0xFD) | ((a == 0) ? 0b10 : 0);
}

void MOS6502::adc_op(uint8_t mode) {
    uint8_t data = op_mode_read(mode);
    if (p & 0x8) {
        uint8_t res = BCD_TO_BIN(a) + BCD_TO_BIN(data) + (p & 1);
        a = BIN_TO_BCD(res);
    } else {
        int16_t res = data + a + (p & 1);
        a = res;
        p = (p & 0x7F) | (a & 0x80);
        p = (p & 0xFE) | ((res > 0xFF) ? 1 : 0);
        p = (p & 0xBF) | (((res > 127) || (res < -128)) ? 0x40 : 0);
    }
    p = (p & 0xFD) | ((a == 0) ? 0b10 : 0);
}

void MOS6502::store_op(uint8_t mode, uint8_t *reg) {
    op_mode_write(mode, *reg);
}

void MOS6502::load_op(uint8_t mode, uint8_t *reg) {
    *reg = op_mode_read(mode);
    p = (p & 0x7F) | (*reg & 0x80);
    p = (p & 0xFD) | ((*reg == 0) ? 0b10 : 0);
}

void MOS6502::cmp_op(uint8_t mode, uint8_t *reg) {
    uint8_t data = op_mode_read(mode);
    p = (p & 0x7F) | (*reg < data);
    p = (p & 0xFD) | ((*reg == data) ? 0b10 : 0);
    p = (p & 0xFE) | ((*reg >= data) ? 1 : 0);
}

void MOS6502::sbc_op(uint8_t mode) {
    uint8_t data = op_mode_read(mode);
    if (p & 0x8) {
        uint8_t res = BCD_TO_BIN(a) - BCD_TO_BIN(data) - (1 - (p & 1));
        a = BIN_TO_BCD(res);
    } else {
        int16_t res = data - a - (1 - (p & 1));
        a = res;
        p = (p & 0x7F) | (a & 0x80);
        p = (p & 0xFE) | ((res >= 0) ? 1 : 0);
        p = (p & 0xBF) | (((res > 127) || (res < -128)) ? 0x40 : 0);
    }
    p = (p & 0xFD) | ((a == 0) ? 0b10 : 0);
}

void MOS6502::branch_op(uint8_t op) {
    int8_t offset = op_mode_read(2);
    switch (op) {
        case 0: pc += (p & 0x80) ? 0 : offset; break;
        case 1: pc += (p & 0x80) ? offset : 0; break;
        case 2: pc += (p & 0x40) ? 0 : offset; break;
        case 3: pc += (p & 0x40) ? offset : 0; break;
        case 4: pc += (p & 0x01) ? 0 : offset; break;
        case 5: pc += (p & 0x01) ? offset : 0; break;
        case 6: pc += (p & 0x02) ? 0 : offset; break;
        case 7: pc += (p & 0x02) ? offset : 0; break;
    }
}

void MOS6502::mod_flags_op(uint8_t op) {
    switch (op) {
        case 0: p &= 0xFE; break;
        case 1: p |= 0x01; break;
        case 2: p &= 0xFB; break;
        case 3: p |= 0x04; break;
        case 5: p &= 0xBF; break;
        case 6: p &= 0xF7; break;
        case 7: p |= 0x08; break;
    }
}

void MOS6502::transf_op(uint8_t *src, uint8_t *dest) {
    *dest = *src;
    p = (p & 0x7F) | (*dest & 0x80);
    p = (p & 0xFD) | ((*dest == 0) ? 0b10 : 0);
}

void MOS6502::brk_op() {
    push16(pc + 2);
    push8(p);
    pc = memory->read16(0xFFFE) - 1;
}

void MOS6502::jsr_op() {
    push16(pc + 3);
    pc = memory->read16(pc + 1) - 1;
}

void MOS6502::rti_op() {
    p = pop8() - 1;
    pc = pop16() - 1;
}

void MOS6502::rts_op() {
    pc = pop16() - 1;
}

void MOS6502::bit_op(uint8_t mode) {
    uint8_t data = op_mode_read(mode);
    p = (p & 0x3F) | (data & 0xC0);
    p = (p & 0xFD) | ((data & a) ? 0 : 0b10);
}

void MOS6502::php_op() {
    push8(p);
}

void MOS6502::plp_op() {
    p = pop8();
}

void MOS6502::pha_op() {
    push8(a);
}

void MOS6502::pla_op() {
    a = pop8();
    p = (p & 0x7F) | (a & 0x80);
    p = (p & 0xFD) | ((a == 0) ? 0b10 : 0);
}

void MOS6502::der_op(uint8_t *reg) {
    (*reg)--;
    p = (p & 0x7F) | (*reg & 0x80);
    p = (p & 0xFD) | ((*reg == 0) ? 0b10 : 0);
}

void MOS6502::inr_op(uint8_t *reg) {
    (*reg)++;
    p = (p & 0x7F) | (*reg & 0x80);
    p = (p & 0xFD) | ((*reg == 0) ? 0b10 : 0);
}

void MOS6502::jmp_op(uint8_t mode) {
    if (mode == 0xFF) {
        pc = memory->read16(memory->read16(pc + 1)) - 1;
    } else {
        pc = memory->read16(pc + 1) - 1;
    }
}

void MOS6502::asl_op(uint8_t mode) {
    if (mode == 2) {
        p = (p & 0xFE) | (a >> 7);
        a <<= 1;
        p = (p & 0x7F) | (a & 0x80);
        p = (p & 0xFD) | ((a == 0) ? 0b10 : 0);
    } else {
        uint8_t data = op_mode_read(mode, false);
        p = (p & 0xFE) | (data >> 7);
        data <<= 1;
        op_mode_write(mode, data);
        p = (p & 0x7F) | (data & 0x80);
        p = (p & 0xFD) | ((data == 0) ? 0b10 : 0);
    }
}

void MOS6502::rol_op(uint8_t mode) {
    if (mode == 2) {
        uint8_t c = a >> 7;
        a = (a << 1) | (p & 1);
        p = (p & 0xFE) | c;
        p = (p & 0x7F) | (a & 0x80);
        p = (p & 0xFD) | ((a == 0) ? 0b10 : 0);
    } else {
        uint8_t data = op_mode_read(mode, false);
        uint8_t c = data >> 7;
        data = (data << 1) | (p & 1);
        op_mode_write(mode, data);
        p = (p & 0xFE) | c;
        p = (p & 0x7F) | (data & 0x80);
        p = (p & 0xFD) | ((data == 0) ? 0b10 : 0);
    }
}

void MOS6502::lsr_op(uint8_t mode) {
    if (mode == 2) {
        p = (p & 0xFE) | (a & 1);
        a >>= 1;
        p = (p & 0x7F) | (a & 0x80);
        p = (p & 0xFD) | ((a == 0) ? 0b10 : 0);
    } else {
        uint8_t data = op_mode_read(mode, false);
        p = (p & 0xFE) | (data & 1);
        data >>= 1;
        op_mode_write(mode, data);
        p = (p & 0x7F) | (data & 0x80);
        p = (p & 0xFD) | ((data == 0) ? 0b10 : 0);
    }
}

void MOS6502::ror_op(uint8_t mode) {
    if (mode == 2) {
        uint8_t c = a & 1;
        a = (a >> 1) | (p << 7);
        p = (p & 0xFE) | c;
        p = (p & 0x7F) | (a & 0x80);
        p = (p & 0xFD) | ((a == 0) ? 0b10 : 0);
    } else {
        uint8_t data = op_mode_read(mode, false);
        uint8_t c = data & 1;
        data = (data >> 1) | (p << 7);
        op_mode_write(mode, data);
        p = (p & 0xFE) | c;
        p = (p & 0x7F) | (data & 0x80);
        p = (p & 0xFD) | ((data == 0) ? 0b10 : 0);
    }
}

void MOS6502::dec_op(uint8_t mode) {
    uint8_t data = op_mode_read(mode, false);
    data--;
    op_mode_write(mode, data);
    p = (p & 0x7F) | (data & 0x80);
    p = (p & 0xFD) | ((data == 0) ? 0b10 : 0);
}

void MOS6502::inc_op(uint8_t mode) {
    uint8_t data = op_mode_read(mode, false);
    data++;
    op_mode_write(mode, data);
    p = (p & 0x7F) | (data & 0x80);
    p = (p & 0xFD) | ((data == 0) ? 0b10 : 0);
}

uint8_t MOS6502::op_mode_read(uint8_t mode, bool inc_pc) {
    uint8_t val;
    switch (mode) {
        case 0: val = pre_ndx_x_read(pc + 1); if (inc_pc) pc++; break;
        case 1: val = zpg_read(pc + 1); if (inc_pc) pc++; break;
        case 2: val = imm_read(pc + 1); if (inc_pc) pc++; break;
        case 3: val = abs_read(pc + 1); if (inc_pc) pc += 2; break;
        case 4: val = post_ndx_y_read(pc + 1); if (inc_pc) pc++; break;
        case 5: val = zpg_x_read(pc + 1); if (inc_pc) pc++; break;
        case 6: val = abs_y_read(pc + 1); if (inc_pc) pc += 2; break;
        case 7: val = abs_x_read(pc + 1); if (inc_pc) pc += 2; break;
        case 8: val = zpg_y_read(pc + 1); if (inc_pc) pc++; break;
    }
    return val;
}

void MOS6502::op_mode_write(uint8_t mode, uint8_t data, bool inc_pc) {
    switch (mode) {
        case 0: pre_ndx_x_write(pc + 1, data); if (inc_pc) pc++; break;
        case 1: zpg_write(pc + 1, data); if (inc_pc) pc++; break;
        case 3: abs_write(pc + 1, data); if (inc_pc) pc += 2; break;
        case 4: post_ndx_y_write(pc + 1, data); if (inc_pc) pc++; break;
        case 5: zpg_x_write(pc + 1, data); if (inc_pc) pc++; break;
        case 6: abs_y_write(pc + 1, data); if (inc_pc) pc += 2; break;
        case 7: abs_x_write(pc + 1, data); if (inc_pc) pc += 2; break;
        case 8: zpg_y_write(pc + 1, data); if (inc_pc) pc++; break;
    }
}

uint8_t MOS6502::imm_read(uint16_t addr) {
    return memory->read8(addr);
}

uint8_t MOS6502::zpg_read(uint16_t addr) {
    return memory->read8(memory->read8(addr));
}

void MOS6502::zpg_write(uint16_t addr, uint8_t data) {
    memory->write8(memory->read8(addr), data);
}

uint8_t MOS6502::zpg_x_read(uint16_t addr) {
    return memory->read8((uint8_t) (memory->read8(addr) + x));
}

uint8_t MOS6502::zpg_y_read(uint16_t addr) {
    return memory->read8((uint8_t) (memory->read8(addr) + y));
}

void MOS6502::zpg_x_write(uint16_t addr, uint8_t data) {
    memory->write8((uint8_t) (memory->read8(addr) + x), data);
}

void MOS6502::zpg_y_write(uint16_t addr, uint8_t data) {
    memory->write8((uint8_t) (memory->read8(addr) + y), data);
}

uint8_t MOS6502::abs_read(uint16_t addr) {
    return memory->read8(memory->read16(addr));
}

void MOS6502::abs_write(uint16_t addr, uint8_t data) {
    memory->write8(memory->read16(addr), data);
}

uint8_t MOS6502::abs_x_read(uint16_t addr) {
    return memory->read8(memory->read16(addr) + x);
}

uint8_t MOS6502::abs_y_read(uint16_t addr) {
    return memory->read8(memory->read16(addr) + y);
}

void MOS6502::abs_x_write(uint16_t addr, uint8_t data) {
    memory->write8(memory->read16(addr) + x, data);
}

void MOS6502::abs_y_write(uint16_t addr, uint8_t data) {
    memory->write8(memory->read16(addr) + y, data);
}

uint8_t MOS6502::pre_ndx_x_read(uint16_t addr) {
    return memory->read8(memory->read16((uint8_t) (memory->read8(addr) + x)));
}

uint8_t MOS6502::post_ndx_y_read(uint16_t addr) {
    return memory->read8(memory->read16(memory->read8(addr)) + y);
}

void MOS6502::pre_ndx_x_write(uint16_t addr, uint8_t data) {
    memory->write8(memory->read16((uint8_t) (memory->read8(addr) + x)), data);
}

void MOS6502::post_ndx_y_write(uint16_t addr, uint8_t data) {
    memory->write8(memory->read16(memory->read8(addr)) + y, data);
}

void MOS6502::push8(uint8_t data) {
    memory->write8(0x100 + sp, data);
    sp--;
}

void MOS6502::push16(uint16_t data) {
    sp-= 2;
    memory->write16(0x100 + sp + 1, data);
}

uint8_t MOS6502::pop8() {
    sp++;
    return memory->read8(0x100 + sp);
}

uint16_t MOS6502::pop16() {
    sp += 2;
    return memory->read16(0x100 + sp - 1);

}