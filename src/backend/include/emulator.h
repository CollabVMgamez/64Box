#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"
#include "cpu.h"
#include "timer.h"
#include "keyboard.h"
#include "video.h"

/* CPU types supported by the emulator. */
typedef enum {
    CPU_8086,
    CPU_8088,
    CPU_80186,
    CPU_386,
    CPU_486
} cpu_type_t;

/* Per-CPU step function type so we can plug different CPU cores. */
typedef void (*cpu_step_fn_t)(cpu_state_t* cpu, memory_t* mem);

/* Emulator state */
typedef struct {
    cpu_type_t      cpu_type;
    bool            running;
    uint32_t        cycles_per_second;
    uint32_t        ram_size;      /* Configurable RAM size in bytes (clamped to MEMORY_SIZE) */
    memory_t        memory;
    cpu_state_t     cpu;
    timer_state_t   timer;
    keyboard_state_t keyboard;
    video_state_t   video;
    cpu_step_fn_t   cpu_step;      /* Selected per CPU type in emulator_init */
} emulator_state_t;

/* Core emulator functions */

/**
 * Initialize the emulator state.
 *
 * cpu_type   - which CPU core to use (8086, 8088, 80186, 386, 486).
 * ram_size   - requested RAM size in bytes; will be clamped to [64KB, MEMORY_SIZE].
 */
bool emulator_init(emulator_state_t* state, cpu_type_t cpu_type, uint32_t ram_size);

void emulator_shutdown(emulator_state_t* state);

/**
 * Run the emulator for the specified number of "cycles".
 * For v0.1, each call to cpu_step() counts as one cycle.
 */
void emulator_run(emulator_state_t* state, uint32_t cycles);

/**
 * Reset the emulator (CPU, memory, and devices) using the previously
 * configured ram_size in the state.
 */
void emulator_reset(emulator_state_t* state);

/* I/O functions */

uint8_t emulator_io_read(emulator_state_t* state, uint16_t port);
void    emulator_io_write(emulator_state_t* state, uint16_t port, uint8_t value);

#endif /* EMULATOR_H */