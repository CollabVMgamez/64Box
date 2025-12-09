#include "emulator.h"
#include "memory.h"
#include "cpu.h"
#include <string.h>
 
#define PIT_PORT_BASE 0x40
#define PIT_PORT_END  0x43
 
/* Very simple keyboard controller I/O ports (PC/AT-style) */
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64
 
/* Helper to clear the simple text-mode video buffer */
static void video_clear(video_state_t* video)
{
    if (!video) {
        return;
    }
 
    memset(video->chars, 0, sizeof(video->chars));
    memset(video->attrs, 0, sizeof(video->attrs));
}
 
bool emulator_init(emulator_state_t* state, cpu_type_t cpu_type, uint32_t ram_size)
{
    if (!state) {
        return false;
    }
 
    state->cpu_type = cpu_type;
    state->running = false;
    state->cycles_per_second = 1000000; /* 1 MHz default */
    state->ram_size = ram_size;
 
    /* Bind the appropriate CPU step function based on CPU type. */
    switch (cpu_type) {
    case CPU_8086:
        state->cpu_step = cpu_8086_step;
        break;
    case CPU_8088:
        state->cpu_step = cpu_8088_step;
        break;
    case CPU_80186:
        state->cpu_step = cpu_80186_step;
        break;
    case CPU_386:
        state->cpu_step = cpu_386_step;
        break;
    case CPU_486:
        state->cpu_step = cpu_486_step;
        break;
    default:
        state->cpu_step = cpu_8086_step;
        break;
    }
 
    /* Initialize memory with the requested RAM size (clamped in memory_init). */
    memory_init(&state->memory, state->ram_size);
    cpu_init(&state->cpu);
    timer_init(&state->timer);
    keyboard_init(&state->keyboard);
    video_clear(&state->video);
 
    return true;
}
 
void emulator_shutdown(emulator_state_t* state)
{
    (void)state;
    /* No dynamic resources yet; placeholder for future cleanup. */
}
 
void emulator_run(emulator_state_t* state, uint32_t cycles)
{
    if (!state) {
        return;
    }
 
    if (cycles == 0) {
        return;
    }
 
    /* Fallback in case cpu_step was not initialized for some reason. */
    if (!state->cpu_step) {
        state->cpu_step = cpu_8086_step;
    }
 
    state->running = true;
 
    uint32_t remaining = cycles;
    while (state->running && remaining-- > 0) {
        /* Execute one instruction using the selected CPU core */
        state->cpu_step(&state->cpu, &state->memory);
 
        /*
         * In a future version, we will account for instruction cycle
         * cost and tick the PIT / other devices accordingly.
         */
    }
}
 
void emulator_reset(emulator_state_t* state)
{
    if (!state) {
        return;
    }
 
    /* Reinitialize memory with the configured RAM size. */
    memory_init(&state->memory, state->ram_size);
    cpu_reset(&state->cpu);
    timer_init(&state->timer);
    keyboard_init(&state->keyboard);
    video_clear(&state->video);
}
 
uint8_t emulator_io_read(emulator_state_t* state, uint16_t port)
{
    if (!state) {
        return 0xFF;
    }
 
    /* PIT / timer ports 0x40-0x43 */
    if (port >= PIT_PORT_BASE && port <= PIT_PORT_END) {
        return timer_read(&state->timer, port);
    }
 
    /* Keyboard controller ports (very simple stub) */
    if (port == KBD_DATA_PORT) {
        /*
         * Return next scan code from the FIFO, or 0 if none.
         * Real hardware behaves differently, but this is good enough
         * for initial testing.
         */
        return keyboard_pop_scancode(&state->keyboard);
    }
 
    if (port == KBD_STATUS_PORT) {
        /*
         * Bit 0 (output buffer status) is set if data is available.
         * We model only that bit; other bits are always 0 for now.
         */
        uint8_t has_data =
            (state->keyboard.head != state->keyboard.tail) ? 0x01 : 0x00;
        return has_data;
    }
 
    /*
     * TODO: Video controller ports
     * TODO: Other chipset / peripheral ports
     */
 
    return 0xFF;
}
 
void emulator_io_write(emulator_state_t* state, uint16_t port, uint8_t value)
{
    if (!state) {
        return;
    }
 
    /* PIT / timer ports 0x40-0x43 */
    if (port >= PIT_PORT_BASE && port <= PIT_PORT_END) {
        timer_write(&state->timer, port, value);
        return;
    }
 
    /* Keyboard controller ports (stubbed) */
    if (port == KBD_DATA_PORT || port == KBD_STATUS_PORT) {
        /*
         * Real 8042 supports commands here; for v0.1 we simply ignore
         * writes to these ports.
         */
        (void)value;
        return;
    }
 
    /*
     * TODO: Video controller ports
     * TODO: Other chipset / peripheral ports
     */
}