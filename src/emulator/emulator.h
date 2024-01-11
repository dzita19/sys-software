#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <stdio.h>
#include "machine/architecture.h"
#include "util/vector.h"

extern FILE* emuin;

#define DEBUG_MODE_OFF   0
#define DEBUG_MODE_SHORT 1
#define DEBUG_MODE_LONG  2

#define MACHINE_REG_COUNT   (8+3)
#define INSTRUCTION_POINTER  8
#define STACK_POINTER        9
#define BASE_POINTER        10
#define MEMORY_SIZE   0x100000U

#define MAX_INSTR_LEN    7

#define PSW_HALT         0
#define PSW_INTR         1

#define PSW_SET_HALT     (program_status_word  |=  (1 << PSW_HALT))
#define PSW_SET_INTR     (program_status_word  |=  (1 << PSW_INTR))

#define PSW_RESET_HALT   (program_status_word  &= ~(1 << PSW_HALT))
#define PSW_RESET_INTR   (program_status_word  &= ~(1 << PSW_INTR))

#define PSW_GET_HALT    ((program_status_word  &   (1 << PSW_HALT)) >> PSW_HALT)
#define PSW_GET_INTR    ((program_status_word  &   (1 << PSW_INTR)) >> PSW_INTR)

#define PSW_ZERO         4
#define PSW_SIGN         5
#define PSW_OVERF        6
#define PSW_CARRY        7

#define PSW_SET_ZERO     (program_status_word  |=  (1 << PSW_ZERO ))
#define PSW_SET_SIGN     (program_status_word  |=  (1 << PSW_SIGN ))
#define PSW_SET_OVERF    (program_status_word  |=  (1 << PSW_OVERF))
#define PSW_SET_CARRY    (program_status_word  |=  (1 << PSW_CARRY))

#define PSW_RESET_ZERO   (program_status_word  &= ~(1 << PSW_ZERO ))
#define PSW_RESET_SIGN   (program_status_word  &= ~(1 << PSW_SIGN ))
#define PSW_RESET_OVERF  (program_status_word  &= ~(1 << PSW_OVERF))
#define PSW_RESET_CARRY  (program_status_word  &= ~(1 << PSW_CARRY))

#define PSW_GET_ZERO    ((program_status_word  &   (1 << PSW_ZERO )) >> PSW_ZERO )
#define PSW_GET_SIGN    ((program_status_word  &   (1 << PSW_SIGN )) >> PSW_SIGN )
#define PSW_GET_OVERF   ((program_status_word  &   (1 << PSW_OVERF)) >> PSW_OVERF)
#define PSW_GET_CARRY   ((program_status_word  &   (1 << PSW_CARRY)) >> PSW_CARRY)

#define INTR_RESET        0
#define INTR_HARD_FAULT   1 // unknown opcode or else
#define INTR_BUS_FAULT    2
#define INTR_ARITHM_FAULT 3 // divide and mod with 0
#define INTR_STACK_OVERF  4

#define INTR_COUNT       16

#define IVT_ADDRESS      0x00000000
#define INITIAL_SP_VAL   STACK_END

#define STATIC_START     0x00000000
#define STATIC_END       HEAP_START

#define HEAP_START       0x00040000
#define HEAP_END         STACK_START

#define STACK_START      0x00080000
#define STACK_END        IO_START

#define IO_START         0x000F0000
#define IO_END           0x00100000

#define TERM_OUT_CTRL    0x000F0000
#define TERM_OUT_STAT    0x000F0004
#define TERM_OUT_VAL     0x000F0008

extern unsigned int   registers[MACHINE_REG_COUNT];
extern unsigned int   program_status_word;
extern unsigned char  memory[MEMORY_SIZE];

extern unsigned int   interrupt_lines;

extern unsigned char  ireg[MAX_INSTR_LEN];
extern AsmInstruction instruction;
extern unsigned int   src_reg; // reg index for reg based addressing, payload for payload based addressing
extern unsigned int   dst_reg;
extern unsigned int   payload;
extern unsigned int   addr_mode;
extern unsigned int   src;
extern unsigned int*  dst;

extern Vector* exec_args;

extern void EmulateProgram(void);

#endif