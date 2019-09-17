#pragma once


#define MEM_INSTR_SIZE  256
#define MEM_DATA_SIZE   256

uint8_t mem_instr[256];
uint8_t mem_data[256];

void mem_reset();
