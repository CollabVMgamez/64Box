#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"
#include "cpu.h"
#include "timer.h"
#include "keyboard.h"
#include "video.h"
 
// CPU types
typedef enum {
    CPU_8086,
    CPU_8088,
    CPU_386,
    CPU_486
} cpu_type_t;
 
/* Per-CPU step function type so we can plug different CPU cores (8086/8088/386/486). */
typedef void (*cpu_step_fn_t)(cpu_state_t* cpu, memory_t* mem);
 
// Emulator state
typedef struct {
    cpu_type_t cpu_type;
    bool running;
    uint32_t cycles_per_second;
    memory_t memory;
    cpu_state_t cpu;
    timer_state_t timer;
    keyboard_state_t keyboard;
    video_state_t video;
    cpu_step_fn_t cpu_step; /* Selected per CPU type in emulator_init */
} emulator_state_t;

// Core emulator functions
bool emulator_init(emulator_state_t* state, cpu_type_t cpu_type);
void emulator_shutdown(emulator_state_t* state);
void emulator_run(emulator_state_t* state, uint32_t cycles);
void emulator_reset(emulator_state_t* state);

// I/O functions
uint8_t emulator_io_read(emulator_state_t* state, uint16_t port);
void emulator_io_write(emulator_state_t* state, uint16_t port, uint8_t value);

#endif // EMULATOR_H