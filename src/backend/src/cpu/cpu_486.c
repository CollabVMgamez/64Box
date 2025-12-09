#include "cpu.h"
#include "memory.h"

/*
 * 80486-specific CPU step.
 *
 * For v0.1 this is a stub that simply reuses the existing 16-bit
 * real-mode decoder implemented in cpu_step() in cpu.c.
 *
 * Proper 486 support will later add:
 *  - 32-bit registers / instructions and flag behaviour
 *  - on-chip cache, FPU integration, etc.
 */

void cpu_486_step(cpu_state_t* cpu, memory_t* mem)
{
    cpu_step(cpu, mem);
}