#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define MEMORY_SIZE 0x100000 // 1MB

typedef struct {
    uint8_t ram[MEMORY_SIZE];
} memory_t;

void memory_init(memory_t* mem);
uint8_t memory_read_byte(memory_t* mem, uint32_t address);
void memory_write_byte(memory_t* mem, uint32_t address, uint8_t value);
uint16_t memory_read_word(memory_t* mem, uint32_t address);
void memory_write_word(memory_t* mem, uint32_t address, uint16_t value);

#endif // MEMORY_H