#include "cpu.h"
#include "memory.h"

/*
 * 8088-specific CPU step.
 *
 * For v0.1, we reuse the same minimal 8086-style decoder implemented in
 * cpu_step() in cpu.c. The 8088 differs mainly in bus width and timing,
 * which we can model later without changing the instruction semantics.
 */

void cpu_8088_step(cpu_state_t* cpu, memory_t* mem)
{
    cpu_step(cpu, mem);
}