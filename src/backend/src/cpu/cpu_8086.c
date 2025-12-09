#include "cpu.h"
#include "memory.h"

/*
 * 8086-specific CPU step.
 *
 * For now this just reuses the generic cpu_step() implementation in cpu.c,
 * which implements a minimal 8086-style instruction subset. As the project
 * grows, this file can diverge to model true 8086 behaviour (timing,
 * quirks, etc.) without affecting other CPU types.
 */

void cpu_8086_step(cpu_state_t* cpu, memory_t* mem)
{
    cpu_step(cpu, mem);
}