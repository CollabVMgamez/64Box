#include "backend_api.h"
#include "emulator.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    emulator_state_t state;
} s64box_instance_t;

static s64box_instance_t* s64box_from_handle(s64box_handle_t handle)
{
    return (s64box_instance_t*)handle;
}

/* Lifecycle */

s64box_handle_t s64box_create(cpu_type_t cpu_type, uint32_t ram_size)
{
    s64box_instance_t* inst = (s64box_instance_t*)malloc(sizeof(s64box_instance_t));
    if (!inst) {
        return NULL;
    }
 
    if (!emulator_init(&inst->state, cpu_type, ram_size)) {
        free(inst);
        return NULL;
    }
 
    return (s64box_handle_t)inst;
}

void s64box_destroy(s64box_handle_t handle)
{
    s64box_instance_t* inst = s64box_from_handle(handle);
    if (!inst) {
        return;
    }

    emulator_shutdown(&inst->state);
    free(inst);
}

bool s64box_reset(s64box_handle_t handle)
{
    s64box_instance_t* inst = s64box_from_handle(handle);
    if (!inst) {
        return false;
    }

    emulator_reset(&inst->state);
    return true;
}

/* Execution */

bool s64box_run_cycles(s64box_handle_t handle, uint32_t cycles)
{
    s64box_instance_t* inst = s64box_from_handle(handle);
    if (!inst) {
        return false;
    }

    emulator_run(&inst->state, cycles);
    return true;
}

/* ROM / memory */

bool s64box_load_rom(s64box_handle_t handle,
                     const uint8_t* data,
                     uint32_t size,
                     uint32_t address)
{
    s64box_instance_t* inst = s64box_from_handle(handle);
    if (!inst || !data) {
        return false;
    }

    memory_t* mem = &inst->state.memory;
    for (uint32_t i = 0; i < size; ++i) {
        memory_write_byte(mem, address + i, data[i]);
    }

    return true;
}

bool s64box_read_memory(s64box_handle_t handle,
                        uint32_t address,
                        uint8_t* out_buffer,
                        uint32_t size)
{
    s64box_instance_t* inst = s64box_from_handle(handle);
    if (!inst || !out_buffer) {
        return false;
    }

    memory_t* mem = &inst->state.memory;
    for (uint32_t i = 0; i < size; ++i) {
        out_buffer[i] = memory_read_byte(mem, address + i);
    }

    return true;
}

bool s64box_write_memory(s64box_handle_t handle,
                         uint32_t address,
                         const uint8_t* data,
                         uint32_t size)
{
    s64box_instance_t* inst = s64box_from_handle(handle);
    if (!inst || !data) {
        return false;
    }

    memory_t* mem = &inst->state.memory;
    for (uint32_t i = 0; i < size; ++i) {
        memory_write_byte(mem, address + i, data[i]);
    }

    return true;
}

/* Introspection */

bool s64box_get_cpu_state(s64box_handle_t handle, cpu_state_t* out_state)
{
    s64box_instance_t* inst = s64box_from_handle(handle);
    if (!inst || !out_state) {
        return false;
    }
 
    memcpy(out_state, &inst->state.cpu, sizeof(cpu_state_t));
    return true;
}
 
bool s64box_get_text_video(s64box_handle_t handle,
                           uint8_t* chars_out,
                           uint8_t* attrs_out,
                           uint32_t cols,
                           uint32_t rows)
{
    s64box_instance_t* inst = s64box_from_handle(handle);
    if (!inst || !chars_out) {
        return false;
    }
 
    if (cols == 0 || rows == 0) {
        return false;
    }
 
    /* Clamp to the emulator's text-mode dimensions */
    if (cols > VIDEO_TEXT_COLS) {
        cols = VIDEO_TEXT_COLS;
    }
    if (rows > VIDEO_TEXT_ROWS) {
        rows = VIDEO_TEXT_ROWS;
    }
 
    video_state_t* vid = &inst->state.video;
 
    for (uint32_t y = 0; y < rows; ++y) {
        for (uint32_t x = 0; x < cols; ++x) {
            uint32_t dst_index = y * cols + x;
            chars_out[dst_index] = vid->chars[y][x];
            if (attrs_out) {
                attrs_out[dst_index] = vid->attrs[y][x];
            }
        }
    }
 
    return true;
}