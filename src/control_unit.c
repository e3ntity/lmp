#include <stdint.h>
#include <string.h>

#include "lmp.h"
#include "alu.h"
#include "control_unit.h"


struct cu_signal cu_decode(uint16_t instr)
{
    /*
        For this implementation, cond gets handled directly in main
    */

    struct cu_signal cu_sig;
    uint8_t op, imm;

    memset((uint8_t *)&cu_sig, '\0', sizeof(struct cu_signal));

    op = (instr & 0x1e00) >> 9;
    imm = (instr & 0x0100) >> 8;

    switch (op)
    {
        case 1: // JMP
            cu_sig.jmp = 1;
            break;
        case 3: // MOV
            cu_sig.imm = imm ? 1 : 0;
            cu_sig.reg_wd_select = 0;
            cu_sig.reg_write = 1;
            break;
        case 4: // LDR
            cu_sig.data_wd_select = imm ? 1 : 0;
            cu_sig.zero_src = 1;
            cu_sig.reg_wd_select = 1;
            cu_sig.reg_write = 1;
            break;
        case 5: // STR
            cu_sig.data_wd_select = imm ? 1 : 0;
            cu_sig.zero_dest = 1;
            cu_sig.mem_write = 1;
            break;
        case 8: // ADD
            cu_sig.imm = imm ? 1 : 0;
            cu_sig.alu_ctrl = ALU_CTRL_ADD;
            cu_sig.reg_wd_select = 2;
            cu_sig.reg_write = 1;
            break;
        case 9: // SUB
            cu_sig.imm = imm ? 1 : 0;
            cu_sig.alu_ctrl = ALU_CTRL_SUB;
            cu_sig.reg_wd_select = 2;
            cu_sig.reg_write = 1;
            break;
        case 10: // LSL
        case 11: // LSR
        case 12: // AND
        case 13: // OR
            break;
    }

    return cu_sig;
}
