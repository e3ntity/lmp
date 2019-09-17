#include <stdint.h>

#include "lmp.h"
#include "register_file.h"
#include "alu.h"

extern uint8_t reg_ic;
extern uint8_t reg_file[];


int8_t alu_resolve(uint8_t alu_ctrl, int8_t a, int8_t b)
{
    int8_t res;

    switch (alu_ctrl)
    {
        case ALU_CTRL_NOP:
            return 0;
        case ALU_CTRL_ADD:
            res = a + b;
            break;
        case ALU_CTRL_SUB:
            res = a - b;
            break;
        default:
            DPRINT("Error: Invalid alu_ctrl %d at %02x\n", alu_ctrl, reg_ic);
            return 0;
    }


    if (res == 0)
        reg_file[REG_FLAGS] |= REG_FLAGS_Z;
    else
        reg_file[REG_FLAGS] ^= REG_FLAGS_Z;

    if (res < 0)
        reg_file[REG_FLAGS] |= REG_FLAGS_N;
    else
        reg_file[REG_FLAGS] ^= REG_FLAGS_N;

    return res;
}
