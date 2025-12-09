#ifndef MEMORY_H
#define MEMORY_H
 
#include <stdint.h>
 
#define MEMORY_SIZE 0x100000 /* 1MB physical limit */
 
typedef struct {
    uint8_t  ram[MEMORY_SIZE];
    uint32_t size;     /* Effective RAM size in bytes (<= MEMORY_SIZE) */
} memory_t;
 
/**
 * Initialize memory to zero and set the effective RAM size.
 *
 * requested_size is clamped to [64KB, MEMORY_SIZE] to avoid degenerate
 * configurations and out-of-bounds access.
 */
void memory_init(memory_t* mem, uint32_t requested_size);
uint8_t memory_read_byte(memory_t* mem, uint32_t address);
void memory_write_byte(memory_t* mem, uint32_t address, uint8_t value);
uint16_t memory_read_word(memory_t* mem, uint32_t address);
void memory_write_word(memory_t* mem, uint32_t address, uint16_t value);
 
#endif // MEMORY_H