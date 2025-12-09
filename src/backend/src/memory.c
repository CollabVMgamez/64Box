#include "memory.h"
#include <string.h>

/**
 * Initialize RAM to zero and configure effective RAM size.
 *
 * requested_size is clamped to [64KB, MEMORY_SIZE].
 */
void memory_init(memory_t* mem, uint32_t requested_size)
{
    if (!mem) {
        return;
    }

    uint32_t min_size = 0x10000u; /* 64KB minimum */
    if (requested_size < min_size) {
        requested_size = min_size;
    }
    if (requested_size > MEMORY_SIZE) {
        requested_size = MEMORY_SIZE;
    }

    mem->size = requested_size;
    memset(mem->ram, 0, sizeof(mem->ram));
}

/**
 * Read a single byte from physical memory.
 * Out-of-range addresses return 0xFF for now.
 */
uint8_t memory_read_byte(memory_t* mem, uint32_t address)
{
    if (!mem) {
        return 0xFF;
    }

    if (address >= mem->size) {
        /* TODO: handle MMIO / out-of-range differently later */
        return 0xFF;
    }

    return mem->ram[address];
}

/**
 * Write a single byte to physical memory.
 * Out-of-range writes are ignored for now.
 */
void memory_write_byte(memory_t* mem, uint32_t address, uint8_t value)
{
    if (!mem) {
        return;
    }

    if (address >= mem->size) {
        /* TODO: handle MMIO / out-of-range differently later */
        return;
    }

    mem->ram[address] = value;
}

/**
 * Read a 16-bit little-endian word from physical memory.
 * If the word would cross the end of RAM, we clamp and compose
 * from available bytes (simple behavior for v0.1).
 */
uint16_t memory_read_word(memory_t* mem, uint32_t address)
{
    if (!mem) {
        return 0xFFFF;
    }

    uint8_t lo = memory_read_byte(mem, address);
    uint8_t hi = memory_read_byte(mem, address + 1);

    return (uint16_t)(lo | (hi << 8));
}

/**
 * Write a 16-bit little-endian word to physical memory.
 */
void memory_write_word(memory_t* mem, uint32_t address, uint16_t value)
{
    if (!mem) {
        return;
    }

    memory_write_byte(mem, address,     (uint8_t)(value & 0xFF));
    memory_write_byte(mem, address + 1, (uint8_t)((value >> 8) & 0xFF));
}