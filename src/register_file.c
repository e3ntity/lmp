#include <stdint.h>

#include "register_file.h"


extern uint8_t reg_ic;
extern uint8_t reg_file[];

void rf_reset()
{
    uint32_t i;

    reg_ic = 0;

    for (i = 0; i < REG_COUNT; i++)
        reg_file[i] = 0;
}
