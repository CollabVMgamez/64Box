#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/*
 * Minimal keyboard controller state for v0.1.
 *
 * For now this is just a tiny FIFO buffer of scan codes. The UI/frontend
 * will be able to push scan codes here; the emulated CPU can read them
 * via I/O ports once we wire that up.
 */

#define KEYBOARD_BUFFER_SIZE 16

typedef struct {
    uint8_t buffer[KEYBOARD_BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
} keyboard_state_t;

/* Initialize keyboard controller state. */
static inline void keyboard_init(keyboard_state_t* kb)
{
    if (!kb) {
        return;
    }

    kb->head = 0;
    kb->tail = 0;
}

/* Push a scan code into the buffer (called from UI/frontend). */
static inline void keyboard_push_scancode(keyboard_state_t* kb, uint8_t scancode)
{
    if (!kb) {
        return;
    }

    uint8_t next_head = (uint8_t)((kb->head + 1) % KEYBOARD_BUFFER_SIZE);
    if (next_head == kb->tail) {
        /* Buffer full, drop the key for now. */
        return;
    }

    kb->buffer[kb->head] = scancode;
    kb->head = next_head;
}

/* Pop a scan code from the buffer, returns 0 if none available. */
static inline uint8_t keyboard_pop_scancode(keyboard_state_t* kb)
{
    if (!kb) {
        return 0;
    }

    if (kb->head == kb->tail) {
        /* Empty */
        return 0;
    }

    uint8_t code = kb->buffer[kb->tail];
    kb->tail = (uint8_t)((kb->tail + 1) % KEYBOARD_BUFFER_SIZE);
    return code;
}

#endif /* KEYBOARD_H */