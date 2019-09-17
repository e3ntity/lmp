#pragma once

#include <stdint.h>

#include "alu.h"


struct cu_signal {
    uint8_t jmp : 1,
            mem_write : 1,
            data_wd_select : 1,
            reg_wd_select : 2,
            imm : 1,
            reg_write : 1,
            zero_dest : 1;
    uint8_t zero_src: 1;
    uint8_t alu_ctrl;
};

struct cu_signal cu_decode(uint16_t);
