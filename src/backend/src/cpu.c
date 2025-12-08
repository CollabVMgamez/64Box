#include "cpu.h"
#include "memory.h"
#include <stdint.h>

/* Initialize CPU state */
void cpu_init(cpu_state_t* cpu) {
    cpu_reset(cpu);
}

/* Reset CPU registers to a simple power-on state (real 8086 reset differs) */
void cpu_reset(cpu_state_t* cpu) {
    cpu->ax = cpu->bx = cpu->cx = cpu->dx = 0;
    cpu->sp = cpu->bp = cpu->si = cpu->di = 0;
    cpu->ip = 0;
    cpu->cs = cpu->ds = cpu->es = cpu->ss = 0;
    cpu->flags = 0;
}

/* Helper to get a pointer to a 16-bit general purpose register by index 0-7 */
static uint16_t* cpu_get_reg16_by_index(cpu_state_t* cpu, uint8_t index) {
    switch (index & 0x7) {
        case 0: return &cpu->ax;
        case 1: return &cpu->cx;
        case 2: return &cpu->dx;
        case 3: return &cpu->bx;
        case 4: return &cpu->sp;
        case 5: return &cpu->bp;
        case 6: return &cpu->si;
        case 7: return &cpu->di;
        default: return &cpu->ax;
    }
}

/* Execute a single instruction at CS:IP */
void cpu_step(cpu_state_t* cpu, memory_t* mem) {
    /* For now we ignore CS and treat IP as a flat 16-bit address */
    uint16_t ip = cpu->ip;
    uint8_t opcode = memory_read_byte(mem, ip++);

    switch (opcode) {
        case 0x90: /* NOP */
            /* Do nothing */
            break;

        /* MOV r16, imm16: opcodes B8-BF */
        case 0xB8: /* MOV AX, imm16 */
        case 0xB9: /* MOV CX, imm16 */
        case 0xBA: /* MOV DX, imm16 */
        case 0xBB: /* MOV BX, imm16 */
        case 0xBC: /* MOV SP, imm16 */
        case 0xBD: /* MOV BP, imm16 */
        case 0xBE: /* MOV SI, imm16 */
        case 0xBF: /* MOV DI, imm16 */
        {
            uint8_t reg_index = opcode - 0xB8;
            uint16_t imm = memory_read_word(mem, ip);
            ip += 2;

            uint16_t* reg = cpu_get_reg16_by_index(cpu, reg_index);
            *reg = imm;
            break;
        }

        /* ADD AX, imm16 (opcode 05) */
        case 0x05:
        {
            uint16_t imm = memory_read_word(mem, ip);
            ip += 2;
            uint32_t result = (uint32_t)cpu->ax + imm;
            cpu->ax = (uint16_t)result;
            /* TODO: update flags properly (CF, ZF, SF, OF, PF, AF) */
            break;
        }

        /* SUB AX, imm16 (opcode 2D) */
        case 0x2D:
        {
            uint16_t imm = memory_read_word(mem, ip);
            ip += 2;
            uint32_t result = (uint32_t)cpu->ax - imm;
            cpu->ax = (uint16_t)result;
            /* TODO: update flags properly */
            break;
        }

        /* JMP rel8 short (opcode EB) */
        case 0xEB:
        {
            int8_t rel = (int8_t)memory_read_byte(mem, ip);
            ip += 1;
            ip = (uint16_t)(ip + rel);
            break;
        }

        /* JMP rel16 near (opcode E9) */
        case 0xE9:
        {
            int16_t rel = (int16_t)memory_read_word(mem, ip);
            ip += 2;
            ip = (uint16_t)(ip + rel);
            break;
        }

        default:
            /*
             * Unknown/unsupported instruction for v0.1.
             * We simply ignore it for now and treat as NOP.
             */
            break;
    }

    cpu->ip = ip;
}