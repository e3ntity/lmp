#pragma once

#include <stdint.h>


#define REG_COUNT 16

uint8_t reg_ic;
uint8_t reg_file[REG_COUNT];

#define REG_FLAGS   0xf
#define REG_SP      0xe
#define REG_BP      0xd
#define REG_R12     0xc
#define REG_R11     0xb
#define REG_R10     0xa
#define REG_R9      0x9
#define REG_R8      0x8
#define REG_R7      0x7
#define REG_R6      0x6
#define REG_R5      0x5
#define REG_R4      0x4
#define REG_R3      0x3
#define REG_R2      0x2
#define REG_R1      0x1
#define REG_R0      0x0

// Flags register
#define REG_FLAGS_N 0x80    // 1000 0000
#define REG_FLAGS_Z 0x40    // 0100 0000

void rf_reset();
