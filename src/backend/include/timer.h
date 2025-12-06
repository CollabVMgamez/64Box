#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef struct {
    uint16_t counter[3];
    uint8_t control;
    uint8_t latch[3];
} timer_state_t;

void timer_init(timer_state_t* timer);
uint8_t timer_read(timer_state_t* timer, uint16_t port);
void timer_write(timer_state_t* timer, uint16_t port, uint8_t value);

#endif // TIMER_H