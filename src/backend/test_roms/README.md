# 64Box Test ROMs (v0.1)

These are tiny hand-crafted binaries that work with the current minimal CPU core implemented in `src/backend/src/cpu.c`.

## Instruction subset currently supported

The CPU core understands a small subset of 8086 instructions:

- `0x90` — `NOP`
- `0xB8`–`0xBF` — `MOV r16, imm16`
  - `0xB8` = `MOV AX, imm16`
  - `0xB9` = `MOV CX, imm16`
  - `0xBA` = `MOV DX, imm16`
  - `0xBB` = `MOV BX, imm16`
  - `0xBC` = `MOV SP, imm16`
  - `0xBD` = `MOV BP, imm16`
  - `0xBE` = `MOV SI, imm16`
  - `0xBF` = `MOV DI, imm16`
- `0x05` — `ADD AX, imm16`
- `0x2D` — `SUB AX, imm16`
- `0xEB` — `JMP rel8` (short relative jump)
- `0xE9` — `JMP rel16` (near relative jump)

There are **no memory read/write instructions yet** (no ModR/M, no `MOV [mem], reg`, etc.), so test ROMs can only manipulate registers and control flow.

---

## ROM 1: Simple increment loop

This ROM:

1. Loads `AX = 0x1234`
2. Adds 1 to `AX` in a loop forever

### Assembly-like pseudocode

Address `0x0000`:

```asm
org 0x0000

start:
    mov ax, 0x1234
loop:
    add ax, 0x0001
    jmp short loop
```

### Machine code bytes

At physical address `0x0000`:

| Offset | Bytes             | Meaning               |
|--------|-------------------|-----------------------|
| 0000h  | B8 34 12          | MOV AX, 0x1234        |
| 0003h  | 05 01 00          | ADD AX, 0x0001        |
| 0006h  | EB FB             | JMP short -5 (to 0003h)|

Flat binary (8 bytes total):

```text
B8 34 12 05 01 00 EB FB
```

### How to create `simple_loop.bin`

Using a hex editor or similar tool, create a binary file with exactly these 8 bytes:

```text
B8 34 12 05 01 00 EB FB
```

Save it as `simple_loop.bin`.

You should then:

- Load it into emulated memory at address `0x0000`.
- Ensure `CS = 0x0000` and `IP = 0x0000` on reset/start.

With the current `cpu_step` implementation, the emulator will:

- Execute `MOV AX,1234h`
- Then repeatedly execute `ADD AX,0001h` and jump back to the ADD
- So `AX` will increase monotonically: `0x1235, 0x1236, 0x1237, ...`

This ROM is ideal for:

- Verifying that:
  - `MOV AX, imm16`
  - `ADD AX, imm16`
  - `JMP rel8`
- behave correctly
- Watching `AX` change over time from the C# UI once P/Invoke wiring is in place.

---

## Usage with the backend API

Once `64box_backend` is built and exposed as a DLL, the expected flow is:

1. Create the emulator:

   ```c
   s64box_handle_t h = s64box_create(CPU_8086);
   ```

2. Load the ROM at `0x0000`:

   ```c
   // rom_data = { 0xB8,0x34,0x12, 0x05,0x01,0x00, 0xEB,0xFB };
   s64box_load_rom(h, rom_data, 8, 0x0000);
   ```

3. Optionally reset:

   ```c
   s64box_reset(h);
   ```

   (For now, you may need to ensure that `CS` and `IP` are set appropriately in the reset implementation.)

4. Run some cycles:

   ```c
   s64box_run_cycles(h, 1000);
   ```

5. Inspect CPU state:

   ```c
   cpu_state_t cpu;
   s64box_get_cpu_state(h, &cpu);
   // cpu.ax should have increased from its initial value
   ```

On the C# side, you will mirror these calls via P/Invoke and can update the UI with the evolving `AX` register value.

---

## Next ROM ideas

Once the instruction set is extended (memory access, conditional jumps, etc.), we can add ROMs that:

- Implement a small countdown loop and stop on a condition.
- Write a pattern into RAM at a known address range that the UI can display.
- Exercise keyboard I/O ports (`0x60`, `0x64`) by polling and reading scan codes.

For now, `simple_loop.bin` is the primary ROM designed to run on the current minimal CPU core.