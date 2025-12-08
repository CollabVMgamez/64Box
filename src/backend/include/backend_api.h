#ifndef BACKEND_API_H
#define BACKEND_API_H

#include <stdint.h>
#include <stdbool.h>
#include "emulator.h"

/*
 * Public C ABI for the 64Box backend, suitable for P/Invoke from C#.
 *
 * v0.1 focuses on:
 *  - Creating/destroying an emulator instance
 *  - Resetting the machine
 *  - Running for a number of cycles
 *  - Loading a simple ROM/binary into memory
 *  - Inspecting CPU state and memory
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle type for the emulator instance from the API consumer's POV. */
typedef void* s64box_handle_t;

/* Lifecycle */

/**
 * Create a new emulator instance with the given CPU type.
 * Returns NULL on failure.
 */
s64box_handle_t s64box_create(cpu_type_t cpu_type);

/**
 * Destroy a previously created emulator instance.
 * After this call, the handle is no longer valid.
 */
void s64box_destroy(s64box_handle_t handle);

/**
 * Reset the emulator (CPU, memory, and basic devices).
 */
bool s64box_reset(s64box_handle_t handle);

/* Execution */

/**
 * Run the emulator for the specified number of cycles.
 * Returns false if the handle is invalid or an internal error occurs.
 */
bool s64box_run_cycles(s64box_handle_t handle, uint32_t cycles);

/* ROM / memory */

/**
 * Load a ROM/binary blob into emulated memory at the given physical address.
 * Typical usage for v0.1 is to load a small test program at 0x0000.
 */
bool s64box_load_rom(s64box_handle_t handle,
                     const uint8_t* data,
                     uint32_t size,
                     uint32_t address);

/**
 * Read a block of memory from the emulator into the provided buffer.
 */
bool s64box_read_memory(s64box_handle_t handle,
                        uint32_t address,
                        uint8_t* out_buffer,
                        uint32_t size);

/**
 * Write a block of memory into the emulator from the provided buffer.
 */
bool s64box_write_memory(s64box_handle_t handle,
                         uint32_t address,
                         const uint8_t* data,
                         uint32_t size);

/* Introspection */

/**
 * Copy the current CPU register state into the provided structure.
 */
bool s64box_get_cpu_state(s64box_handle_t handle, cpu_state_t* out_state);

/**
 * Copy the current 80x25 text-mode video buffer into the provided
 * character and attribute arrays. The buffers must be at least
 * cols * rows bytes in size.
 *
 * If attrs_out is NULL, only characters are returned.
 */
bool s64box_get_text_video(s64box_handle_t handle,
                           uint8_t* chars_out,
                           uint8_t* attrs_out,
                           uint32_t cols,
                           uint32_t rows);

#ifdef __cplusplus
}
#endif

#endif /* BACKEND_API_H */