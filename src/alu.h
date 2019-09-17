#pragma once

#include <stdint.h>

#define ALU_CTRL_NOP    0x00
#define ALU_CTRL_ADD    0x01
#define ALU_CTRL_SUB    0x02

int8_t alu_resolve(uint8_t, int8_t, int8_t);
