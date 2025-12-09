#include "cpu.h"
#include "memory.h"

/*
 * 80186-specific CPU step.
 *
 * For v0.1 this simply reuses the existing 16-bit real-mode decoder
 * implemented in cpu_step() in cpu.c. Later we can extend this path
 * to model 80186-specific instructions and quirks without affecting
 * the other CPU types.
 */

void cpu_80186_step(cpu_state_t* cpu, memory_t* mem)
{
    cpu_step(cpu, mem);
}