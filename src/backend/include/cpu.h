#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "memory.h"

/**
 * Basic 16-bit x86 CPU state for 8086/8088-style processors.
 *
 * v0.1 goal: real-mode only, minimal instruction set.
 */
typedef struct {
    /* General purpose registers */
    uint16_t ax;
    uint16_t bx;
    uint16_t cx;
    uint16_t dx;

    /* Index / pointer registers */
    uint16_t sp;
    uint16_t bp;
    uint16_t si;
    uint16_t di;

    /* Instruction pointer */
    uint16_t ip;

    /* Segment registers */
    uint16_t cs;
    uint16_t ds;
    uint16_t es;
    uint16_t ss;

    /* Flags register */
    uint16_t flags;
} cpu_state_t;

/**
 * Initialize the CPU state.
 * This should be called once when the emulator is created.
 */
void cpu_init(cpu_state_t* cpu);

/**
 * Reset the CPU state to its power-on / reset vector.
 */
void cpu_reset(cpu_state_t* cpu);

/**
 * Execute a single instruction at the current CS:IP.
 *
 * This is the generic 8086-style decoder used as a core implementation
 * that per-CPU step functions can delegate to.
 */
void cpu_step(cpu_state_t* cpu, memory_t* mem);

/* Per-CPU step entry points (to be bound via emulator_state_t.cpu_step). */
void cpu_8086_step(cpu_state_t* cpu, memory_t* mem);
void cpu_8088_step(cpu_state_t* cpu, memory_t* mem);
void cpu_80186_step(cpu_state_t* cpu, memory_t* mem);
void cpu_386_step(cpu_state_t* cpu, memory_t* mem);
void cpu_486_step(cpu_state_t* cpu, memory_t* mem);

#endif // CPU_H