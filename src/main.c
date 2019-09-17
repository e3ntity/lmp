#include <stdint.h>
#include <time.h>

#include "register_file.h"
#include "memory.h"
#include "control_unit.h"
#include "lmp.h"

extern uint8_t reg_ic;
extern uint8_t reg_file[];
extern uint8_t mem_instr[];
extern uint8_t mem_data[];


void handle(uint16_t);
int64_t time_passed(clockid_t, struct timespec);
#define TIME_PASSED(x) time_passed(CLOCK_MONOTONIC, x)

void print_mem(uint32_t start, uint32_t count)
{
    #ifdef DEBUG
      uint32_t i;

      printf("Data Memory:\n");
      for (i = start; i < count; i++)
      {
        printf("%02x", mem_data[i]);

        if (i % 8 == 7) printf("\n");
      }
      printf("\nRegisters:\n");
      for (i = 0; i < REG_COUNT; i++)
        printf("R%02d: %02x\n", i, reg_file[i]);
    #endif

    return;
}

int32_t main(uint32_t argc, uint8_t **argv)
{
    struct timespec clock_start;
    uint16_t *instruction;

    DPRINT("Running Lukas' Microprocessor (LMP) emulation\n");

    // Reset
    rf_reset();
    mem_reset();

    /*

        Program for first 10 Fibonacci numbers:

        Assembly                Binary                  Bytes (Little-Endian)
        
        0x00: MOV r0, 0         0000 0111 0000 0000     \x00\x07
        0x02: MOV r1, 1         0000 0111 0001 0001     \x11\x07
        0x04: MOV r3, #10       0000 0111 0011 1010     \x3a\x07
        L1
        0x06: ADD r0, r1        0001 0000 0000 0001     \x01\x10
        0x08: STR 0             0000 1011 0000 0000     \x00\x0b
        0x0a: MOV r2, r0        0000 0110 0010 0000     \x20\x06
        0x0c: MOV r0, r1        0000 0110 0000 0001     \x01\x06
        0x0e: MOV r1, r2        0000 0110 0001 0010     \x12\x06
        0x10: SUB r3, #1        0001 0011 0011 0001     \x31\x13
        0x12: JNE L1            1100 0010 0000 0110     \x06\xc2
        L2
        0x14: JMP L2            0000 0010 0001 0100     \x14\x02

    */

    // Load Program
    uint8_t prog[] = "\x00\x07\x11\x07\x3a\x07\x01\x10\x00\x0b\x20\x06\x01\x06\x12\x06\x31\x13\x06\xc2\x14\x02";
    for (uint32_t i = 0; i < 22; i++)
        *(mem_instr + i) = prog[i];

    while (1)
    {
        // Set clock
        clock_gettime(CLOCK_MONOTONIC, &clock_start);

        print_mem(0, 32);

        // Fetch instruction (Instruction Memory)
        instruction = (uint16_t *)(mem_instr + reg_ic);

        DPRINT(
            "0x%02x: \\x%02x\\x%02x\n",
            reg_ic,
            (*instruction >> 8) & 0xff,
            *instruction & 0xff
        );

        handle(*instruction);

        // Wait for clock
        while (TIME_PASSED(clock_start) < CLOCK_PERIOD_NSEC);
    }


    return 0;
}

bool cond_verify(uint16_t instr)
{
    switch((instr & 0xe000) >> 13)
    {
        case 0: return true;
        case 1: 
            return (!GET_BIT(reg_file[REG_FLAGS], REG_FLAGS_N) &&
                    !GET_BIT(reg_file[REG_FLAGS], REG_FLAGS_Z));
        case 2:
            return GET_BIT(reg_file[REG_FLAGS], REG_FLAGS_N);
        case 3:
            return !GET_BIT(reg_file[REG_FLAGS], REG_FLAGS_N);
        case 4:
            return (GET_BIT(reg_file[REG_FLAGS], REG_FLAGS_N) ||
                    GET_BIT(reg_file[REG_FLAGS], REG_FLAGS_Z));
        case 5:
            return GET_BIT(reg_file[REG_FLAGS], REG_FLAGS_Z);
        case 6:
            return !GET_BIT(reg_file[REG_FLAGS], REG_FLAGS_Z);
        default:
            DPRINT("Error: Invalid cond at %02x\n", reg_ic);
            // TODO: ERROR
            return false;
    }
}

void handle(uint16_t instr)
{
    struct cu_signal cu_sig;
    uint8_t next_ic;                // IC'
    uint8_t rf_a1, rf_a2, rf_wd;    // Register File inputs
    uint8_t rf_rd1, rf_rd2;         // Register File outputs
    uint8_t dm_a;                   // Data Memory input
    uint8_t dm_rd;                  // Data Memory output
    uint8_t alu_res;                // ALU result

    // This should normally be handled in the CU
    if (!cond_verify(instr))
    {
        reg_ic += 2;
        return;
    }

    // A: Resolve Signals

    cu_sig = cu_decode(instr);

    /*printf("0D: %d\n", cu_sig.zero_dest);
    printf("0S: %d\n", cu_sig.zero_src);
    printf("RW: %d\n", cu_sig.reg_write);
    printf("IMM: %d\n", cu_sig.imm);
    printf("SRC_SEL: %d\n", cu_sig.reg_wd_select);*/

    // Calculate PC'
    if (cu_sig.jmp)
        next_ic = (instr & 0xff);
    else next_ic = reg_ic + 2;

    // Register File A1 and A2
    if (!cu_sig.zero_dest)
        rf_a1 = (instr & 0xf0) >> 4;
    else rf_a1 = 0;

    if (!cu_sig.zero_src)
        rf_a2 = (instr & 0x0f);
    else rf_a2 = 0;

    // Resolve RD1 and RD2
    rf_rd1 = reg_file[rf_a1];
    rf_rd2 = reg_file[rf_a2];

    // Resolve VB
    if (cu_sig.imm)
        rf_rd2 = (instr & 0xf);

    // Calculate ALU Result
    alu_res = (uint8_t)alu_resolve(cu_sig.alu_ctrl, (int8_t)rf_rd1, (int8_t)rf_rd2);

    // Data Memory A & RD
    if (cu_sig.data_wd_select)
        dm_a = (instr & 0xff);
    else
        dm_a = rf_rd2;

    dm_rd = mem_data[dm_a];

    // Resolve Register File WD signal
    switch (cu_sig.reg_wd_select)
    {
        case 0:
            rf_wd = rf_rd2;
            break;
        case 1:
            rf_wd = dm_rd;
            break;
        case 2:
            rf_wd = alu_res;
            break;
        default:
            DPRINT("Error: Invalid reg_wd_select signal %d at %d\n", cu_sig.reg_wd_select, reg_ic);
    }

    // B: Write Signals
    if (cu_sig.mem_write)
        mem_data[dm_a] = rf_rd2;

    if (cu_sig.reg_write)
        reg_file[rf_a1] = rf_wd;

    reg_ic = next_ic;

    return;
}

int64_t time_passed(clockid_t clk_id, struct timespec start)
{
    /*
        Calculation taken from https://stackoverflow.com/a/6749766
    */

    int64_t diff;
    struct timespec now;

    clock_gettime(clk_id, &now);

    if ((now.tv_nsec - start.tv_nsec) < 0) {
        diff = (now.tv_sec - start.tv_sec - 1);
        diff *= 1000000000;
        diff += 1000000000;
        diff += now.tv_nsec - start.tv_nsec;
    } else {
        diff = now.tv_sec - start.tv_sec;
        diff *= 1000000000;
        diff += now.tv_nsec - start.tv_nsec;
    }

    return diff;
}
