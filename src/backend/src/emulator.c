#include "emulator.h"
#include "memory.h"
#include "cpu.h"

bool emulator_init(emulator_state_t* state, cpu_type_t cpu_type) {
    state->cpu_type = cpu_type;
    state->running = false;
    state->cycles_per_second = 1000000; // 1MHz
    memory_init(&state->memory);
    cpu_init(&state->cpu);
    return true;
}

void emulator_shutdown(emulator_state_t* state) {
    // Cleanup
}

void emulator_run(emulator_state_t* state, uint32_t cycles) {
    // TODO: Run CPU
}

void emulator_reset(emulator_state_t* state) {
    memory_init(&state->memory);
    cpu_reset(&state->cpu);
}