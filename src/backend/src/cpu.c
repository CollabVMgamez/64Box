#include "cpu.h"
#include "memory.h"

void cpu_init(cpu_state_t* cpu) {
    cpu_reset(cpu);
}

void cpu_reset(cpu_state_t* cpu) {
    cpu->ax = cpu->bx = cpu->cx = cpu->dx = 0;
    cpu->sp = cpu->bp = cpu->si = cpu->di = 0;
    cpu->ip = 0;
    cpu->cs = cpu->ds = cpu->es = cpu->ss = 0;
    cpu->flags = 0;
}

void cpu_step(cpu_state_t* cpu, memory_t* mem) {
    uint8_t opcode = memory_read_byte(mem, cpu->ip);
    cpu->ip++;

    switch (opcode) {
        case 0x90: // NOP
            break;
        case 0xB8: // MOV AX, imm16
            cpu->ax = memory_read_word(mem, cpu->ip);
            cpu->ip += 2;
            break;
        case 0xB9: // MOV CX, imm16
            cpu->cx = memory_read_word(mem, cpu->ip);
            cpu->ip += 2;
            break;
        default:
            // Unknown instruction - halt for now
            break;
    }
}