# 64Box

64Box is an experimental PC emulator inspired by 86Box.
Version: **0.1 (work in progress)**

- **Backend:** C (CPU, memory, devices, emulator core) – built as a native DLL.
- **Frontend:** C# WinForms – built with the `dotnet` CLI.
- **Target CPUs (planned):** 8086, 8088, 386, 486.
  v0.1 focuses on a small 8086/8088-style core with a minimal instruction subset.

---

## Project layout

- Backend (`C`):

  - [`emulator.h`](src/backend/include/emulator.h:1) – main emulator state [`emulator_state_t`](src/backend/include/emulator.h:20), CPU type enum, run/reset/IO APIs.
  - [`cpu.h`](src/backend/include/cpu.h:1) – CPU register state [`cpu_state_t`](src/backend/include/cpu.h:17), `cpu_init`, `cpu_reset`, `cpu_step`.
  - [`memory.h`](src/backend/include/memory.h:1) – 1 MB RAM definition [`memory_t`](src/backend/include/memory.h:8) and byte/word access APIs.
  - [`timer.h`](src/backend/include/timer.h:1) – PIT-like timer state and IO helpers.
  - [`keyboard.h`](src/backend/include/keyboard.h:1) – simple scan-code FIFO for keyboard input.
  - [`video.h`](src/backend/include/video.h:1) – minimal 80×25 text-mode video buffer [`video_state_t`](src/backend/include/video.h:17).
  - [`backend_api.h`](src/backend/include/backend_api.h:1) – public C ABI (`s64box_*` functions) exported from the backend DLL.

  - [`emulator.c`](src/backend/src/emulator.c:1) – init/reset/run loop, IO port dispatch, device wiring.
  - [`cpu.c`](src/backend/src/cpu.c:1) – current minimal 8086-style instruction decoder (`cpu_step`).
  - [`memory.c`](src/backend/src/memory.c:1) – RAM implementation.
  - [`timer.c`](src/backend/src/timer.c:1) – PIT-style timer stub.
  - [`backend_api.c`](src/backend/src/backend_api.c:1) – implements the exported `s64box_*` functions used by C#.

  - [`CMakeLists.txt`](src/backend/CMakeLists.txt:1) – builds the backend as `64box_backend` (DLL).

  - [`test_roms/README.md`](src/backend/test_roms/README.md:1) – documentation for small hand-written test binaries (e.g. `simple_loop.bin`).

- Frontend (`C#`, WinForms):

  - [`SixtyFourBox.UI.csproj`](src/ui/SixtyFourBox.UI.csproj:1) – .NET project file for the UI.
  - [`Program.cs`](src/ui/Program.cs:1) – WinForms entry point.
  - [`MainForm.cs`](src/ui/MainForm.cs:1) – main UI:
    - CPU type selection (8086/8088/386/486).
    - ROM loading.
    - Start/Stop/Reset controls.
    - Register log.
    - 80×25 text-mode display backed by the backend `video_state_t`.

---

## Prerequisites

You’ll need:

- **C toolchain on Windows**
  - e.g. MSVC / Visual Studio Build Tools (x64 recommended).
- **CMake** installed and available on `PATH`:
  - `cmake --version` should work in your terminal.
- **.NET SDK** (8.x, already present on the dev machine) for the UI:
  - `dotnet --version` should report a recent SDK.

Repository root is assumed to be:

```text
c:\Users\Robert\Downloads\64Box-main
```

---

## Building the backend DLL (`64box_backend.dll`)

1. Open a terminal where `cmake --version` works.
2. Run:

```bat
cd c:\Users\Robert\Downloads\64Box-main\src\backend

cmake -S . -B build
cmake --build build --config Debug
```

This uses [`CMakeLists.txt`](src/backend/CMakeLists.txt:1) to compile:

- [`emulator.c`](src/backend/src/emulator.c:1)
- [`cpu.c`](src/backend/src/cpu.c:1)
- [`memory.c`](src/backend/src/memory.c:1)
- [`timer.c`](src/backend/src/timer.c:1)
- [`backend_api.c`](src/backend/src/backend_api.c:1)

Key CMake snippet:

```cmake
add_library(64box_backend SHARED
    src/emulator.c
    src/cpu.c
    src/memory.c
    src/timer.c
    src/backend_api.c
)

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
```

The result is a DLL named `64box_backend.dll`.

Typical output locations:

- `src\backend\build\Debug\64box_backend.dll` (MSVC multi-config)
- `src\backend\build\64box_backend.dll` (single-config generators)

Adjust paths below if your generator differs.

---

## Making the DLL visible to the UI

The C# UI uses:

```csharp
[DllImport("64box_backend", CallingConvention = CallingConvention.Cdecl)]
```

(see [`Native64Box`](src/ui/MainForm.cs:9)), so Windows searches for `64box_backend.dll` on the DLL search path.

Simplest approach: copy the DLL next to the UI binary:

```bat
copy c:\Users\Robert\Downloads\64Box-main\src\backend\build\Debug\64box_backend.dll ^
     c:\Users\Robert\Downloads\64Box-main\src\ui\bin\Debug\net6.0-windows\
```

After this, `src\ui\bin\Debug\net6.0-windows\` should contain:

- `SixtyFourBox.UI.dll`
- `64box_backend.dll`
- other .NET runtime files

---

## Building and running the C# UI

From the UI directory:

```bat
cd c:\Users\Robert\Downloads\64Box-main\src\ui
dotnet build
dotnet run
```

The project [`SixtyFourBox.UI.csproj`](src/ui/SixtyFourBox.UI.csproj:1):

- Targets `net6.0-windows`.
- Uses `Microsoft.NET.Sdk.WindowsDesktop`.
- Has `<UseWindowsForms>true</UseWindowsForms>`.

You’ll see warnings that `net6.0-windows` is out of support and that `Microsoft.NET.Sdk.WindowsDesktop` isn’t strictly required; these can be ignored for now.

Runtime behavior:

- If `64box_backend.dll` **is present** and loadable:
  - [`MainForm`](src/ui/MainForm.cs:74) calls `Native64Box.s64box_create` in its backend initialization.
  - The log shows something like:

    ```text
    [HH:MM:SS] Emulator created with CPU type CPU_8086.
    ```

  - Start/Stop/Reset/Load ROM remain enabled.

- If `64box_backend.dll` **is missing**:
  - `CreateEmulator()` catches `DllNotFoundException`.
  - Logs that the backend DLL isn’t found and disables backend-dependent controls.
  - The UI window still opens so you can see the message.

---

## Supported instruction subset (v0.1 CPU core)

The current 16-bit core in [`cpu_step`](src/backend/src/cpu.c:35) implements a very small subset of 8086 instructions:

- `0x90` – `NOP`
- `0xB8`–`0xBF` – `MOV r16, imm16`
  - AX, CX, DX, BX, SP, BP, SI, DI
- `0x05` – `ADD AX, imm16`
- `0x2D` – `SUB AX, imm16`
- `0xEB` – `JMP rel8` (short)
- `0xE9` – `JMP rel16` (near)

No memory addressing or stack/interrupt instructions yet—only register operations and relative jumps.

---

## Example ROM: `simple_loop.bin`

Documented in more detail in [`test_roms/README.md`](src/backend/test_roms/README.md:1).

Pseudo-assembly:

```asm
org 0x0000

start:
    mov ax, 0x1234
loop:
    add ax, 0x0001
    jmp short loop
```

Raw bytes at offset `0x0000`:

```text
B8 34 12 05 01 00 EB FB
```

Create a binary file `simple_loop.bin` containing exactly those 8 bytes.

Steps to run:

1. Build the backend and copy `64box_backend.dll` as described above.
2. Build and run the UI:

   ```bat
   cd c:\Users\Robert\Downloads\64Box-main\src\ui
   dotnet run
   ```

3. In the UI:
   - Select CPU type **8086**.
   - Click **“Load ROM…”** and choose `simple_loop.bin`
     → calls `s64box_load_rom` to put the ROM at address `0x0000`.
   - Click **“Start”**.

On each tick:

- [`MainForm.OnRunTimerTick`](src/ui/MainForm.cs:327) calls:
  - `s64box_run_cycles` with a small cycle batch.
  - `s64box_get_cpu_state` to dump registers (AX, BX, CX, DX, IP, CS).
  - `s64box_get_text_video` to update the 80×25 text-mode display.

You should see AX increasing over time in the log.

---

## Text-mode display

The backend holds an 80×25 text buffer in [`video_state_t`](src/backend/include/video.h:17):

- `chars[VIDEO_TEXT_ROWS][VIDEO_TEXT_COLS]` – character codes.
- `attrs[VIDEO_TEXT_ROWS][VIDEO_TEXT_COLS]` – attributes (not yet interpreted in the UI).

The C ABI exposes this via:

- [`s64box_get_text_video`](src/backend/include/backend_api.h:94) implemented in [`backend_api.c`](src/backend/src/backend_api.c:127).

The UI:

- Has a multiline `_videoTextBox` in [`MainForm`](src/ui/MainForm.cs:74) with a monospaced font.
- On each tick, calls `s64box_get_text_video` for `cols=80`, `rows=25`.
- Renders the returned buffer into `_videoTextBox`.

At the moment, nothing writes into the video buffer, so it appears blank, but the plumbing from backend → UI is in place for future text output.

---

## Roadmap (short term)

Planned improvements:

- **CPU families:**
  - Split CPU cores into per-type files (e.g. `cpu_8086.c`, `cpu_8088.c`, `cpu_386.c`, `cpu_486.c`) under a `cpu` folder.
  - Use a function pointer (`cpu_step_fn_t`) in [`emulator_state_t`](src/backend/include/emulator.h:20) to select the correct step function based on `cpu_type_t`.

- **Instruction set expansion:**
  - Add more 8086 instructions (memory operands, stack, conditional branches, flag handling).

- **ROM & boot flow:**
  - Improve reset vector behavior and ROM mapping for a more realistic startup path.

- **Video mapping:**
  - Map a RAM region (e.g. `0xB8000`) into the text buffer so writing to “video memory” updates the UI display.

- **Testing:**
  - Add instruction-level tests and more sample ROMs in [`test_roms`](src/backend/test_roms/README.md:1).

This README should give you enough to build, run, and start experimenting with the current 64Box v0.1 implementation, and it documents how the pieces (backend DLL + C# UI) fit together.
