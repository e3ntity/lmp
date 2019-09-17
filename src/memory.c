#include <stdint.h>

#include "memory.h"


extern uint8_t mem_instr[];
extern uint8_t mem_data[];

void mem_reset()
{
    uint32_t i;

    for (i = 0; i < 256; i++)
        *(mem_instr + i) = '\0';

    for (i = 0; i < 256; i++)
        *(mem_data + i) = '\0';
}
