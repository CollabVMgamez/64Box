#include "timer.h"
#include <string.h>

/*
 * Very simple PIT-like timer stub for v0.1.
 *
 * We model only the registers and basic read/write behavior on the
 * classic PC PIT I/O ports 0x40-0x43. No accurate timing yet.
 */

#define PIT_COUNTER0_PORT 0x40
#define PIT_COUNTER1_PORT 0x41
#define PIT_COUNTER2_PORT 0x42
#define PIT_CONTROL_PORT  0x43

void timer_init(timer_state_t* timer)
{
    if (!timer) {
        return;
    }

    memset(timer->counter, 0, sizeof(timer->counter));
    memset(timer->latch, 0, sizeof(timer->latch));
    timer->control = 0;
}

/*
 * Read timer register via I/O port.
 * For v0.1, we just return the current counter low byte.
 */
uint8_t timer_read(timer_state_t* timer, uint16_t port)
{
    if (!timer) {
        return 0xFF;
    }

    switch (port) {
    case PIT_COUNTER0_PORT:
        return (uint8_t)(timer->counter[0] & 0xFF);
    case PIT_COUNTER1_PORT:
        return (uint8_t)(timer->counter[1] & 0xFF);
    case PIT_COUNTER2_PORT:
        return (uint8_t)(timer->counter[2] & 0xFF);
    case PIT_CONTROL_PORT:
        return timer->control;
    default:
        /* Not a PIT port; in a fuller system this would be handled elsewhere. */
        return 0xFF;
    }
}

/*
 * Write timer register via I/O port.
 * We record the written values but do not yet simulate tick behavior.
 */
void timer_write(timer_state_t* timer, uint16_t port, uint8_t value)
{
    if (!timer) {
        return;
    }

    switch (port) {
    case PIT_COUNTER0_PORT:
        timer->counter[0] = (timer->counter[0] & 0xFF00) | value;
        break;
    case PIT_COUNTER1_PORT:
        timer->counter[1] = (timer->counter[1] & 0xFF00) | value;
        break;
    case PIT_COUNTER2_PORT:
        timer->counter[2] = (timer->counter[2] & 0xFF00) | value;
        break;
    case PIT_CONTROL_PORT:
        timer->control = value;
        break;
    default:
        /* Not a PIT port; ignore for now. */
        break;
    }
}