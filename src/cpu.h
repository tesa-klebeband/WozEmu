#ifndef CPU_H
#define CPU_H

#include <cstdint>

#include "memory.h"

class MOS6502 {
    public:
        MOS6502(Memory *memory);
        void reset();
        void cycle();

    private:
        void bitwise_op(uint8_t mode, uint8_t op);
        void adc_op(uint8_t mode);
        void store_op(uint8_t mode, uint8_t *reg);
        void load_op(uint8_t mode, uint8_t *reg);
        void cmp_op(uint8_t mode, uint8_t *reg);
        void sbc_op(uint8_t mode);
        void branch_op(uint8_t mode);
        void mod_flags_op(uint8_t mode);
        void transf_op(uint8_t *src, uint8_t *dest);
        void brk_op();
        void jsr_op();
        void rti_op();
        void rts_op();
        void bit_op(uint8_t mode);
        void php_op();
        void plp_op();
        void pha_op();
        void pla_op();
        void der_op(uint8_t *reg);
        void inr_op(uint8_t *reg);
        void jmp_op(uint8_t mode);
        void asl_op(uint8_t mode);
        void rol_op(uint8_t mode);
        void lsr_op(uint8_t mode);
        void ror_op(uint8_t mode);
        void dec_op(uint8_t mode);
        void inc_op(uint8_t mode);
        uint8_t op_mode_read(uint8_t mode, bool inc_pc = true);
        void op_mode_write(uint8_t mode, uint8_t data, bool inc_pc = true);
        uint8_t imm_read(uint16_t addr);
        uint8_t zpg_read(uint16_t addr);
        void zpg_write(uint16_t addr, uint8_t data);
        uint8_t zpg_x_read(uint16_t addr);
        uint8_t zpg_y_read(uint16_t addr);
        void zpg_x_write(uint16_t addr, uint8_t data);
        void zpg_y_write(uint16_t addr, uint8_t data);
        uint8_t abs_read(uint16_t addr);
        void abs_write(uint16_t addr, uint8_t data);
        uint8_t abs_x_read(uint16_t addr);
        uint8_t abs_y_read(uint16_t addr);
        void abs_x_write(uint16_t addr, uint8_t data);
        void abs_y_write(uint16_t addr, uint8_t data);
        uint8_t pre_ndx_x_read(uint16_t addr);
        uint8_t post_ndx_y_read(uint16_t addr);
        void pre_ndx_x_write(uint16_t addr, uint8_t data);
        void post_ndx_y_write(uint16_t addr, uint8_t data);
        void push8(uint8_t data);
        void push16(uint16_t data);
        uint8_t pop8();
        uint16_t pop16();

        uint8_t a, x, y, sp, p;
        uint16_t pc;
        Memory *memory;
};

#endif