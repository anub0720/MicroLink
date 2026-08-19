# MicroLink — 8-bit Embedded Computer Simulator

````
# MicroLink

MicroLink is a small, from-scratch **8-bit embedded computer simulator** written in modern C++.

The project models the major pieces of a simple embedded computer:

- 8-bit CPU
- 16-bit address space
- 64 KiB memory
- Instruction decoder
- Instruction executor
- Registers and CPU state
- Stack operations
- Arithmetic and logical operations
- Branching and subroutines
- Memory-mapped I/O
- GPIO peripheral
- 8-bit LED visualization
- UART serial output
- Hardware timer
- Execution tracing
- Step/run/reset control
- REST API using Crow
- Interactive React frontend
- Automated C++ tests using GoogleTest
- CMake + Ninja build system

The goal is not to emulate an existing commercial CPU.

Instead, MicroLink is a deliberately designed educational/engineering project that demonstrates how an embedded processor can be built from the ground up and exposed through a usable debugging interface.

---

# 1. Project Status

## Version 1 — Functional

MicroLink V1 currently provides a working simulated embedded computer with:

- CPU execution
- instruction fetch/decode/execute
- 8-bit registers/data
- 16-bit program counter/addressing
- 64 KiB memory
- stack support
- branching
- arithmetic/logic instructions
- memory-mapped peripherals
- GPIO
- UART
- timer
- execution trace
- single-step execution
- bounded execution
- program loading
- CPU reset
- REST API
- React visualization
- LED visualization
- UART terminal
- timer visualization
- memory viewer
- built-in demonstration programs
- custom GPIO patterns
- automated tests

The project is intentionally **not a complete real-world microcontroller**.

It is a compact V1 embedded-computer platform intended to demonstrate CPU architecture, low-level programming concepts, hardware abstraction, device buses, memory-mapped I/O, runtime execution, testing, and frontend visualization.

---

# 2. Why MicroLink?

MicroLink was designed to answer a practical engineering question:

> What does it actually take to build and visualize a small 8-bit embedded computer?

Instead of only implementing a CPU instruction emulator, MicroLink connects the entire stack:

```text
User Interface
      │
      ▼
REST API / Crow
      │
      ▼
Application Layer
      │
      ▼
Runtime / Controller
      │
      ▼
CPU
 ┌────┼──────────────┐
 ▼    ▼              ▼
Memory Executor   CPU State
 │
 ▼
Device Bus
 ├── GPIO
 ├── UART
 └── Timer
````

This makes the project closer to a small embedded platform than a simple instruction-set demo.

---

# 3. Main Features

## CPU

MicroLink implements a custom 8-bit CPU architecture.

The CPU contains:

- Program Counter
- Accumulator
- Stack Pointer
- Instruction Register
- CPU status/state
- Memory interface
- Instruction fetch/decode/execute cycle

The CPU follows the basic model:

```
FETCH
  ↓
DECODE
  ↓
EXECUTE
  ↓
UPDATE STATE
  ↓
NEXT INSTRUCTION
```

---

# 4. Address Space

The CPU uses a 16-bit address space.

Therefore:

```
2^16 = 65536 bytes
```

The total addressable memory is:

```
64 KiB
```

Memory addresses range from:

```
0x0000
```

to:

```
0xFFFF
```

A portion of the address space is reserved for memory-mapped peripherals.

Current device addresses include:

```
GPIO
0xF000  GPIO data
0xF001  GPIO direction

UART
0xF010  UART interface

TIMER
0xF020  timer counter
0xF021  timer compare
0xF022  timer control
```

The exact device implementation is intentionally simple in V1.

---

# 5. Instruction Set

MicroLink uses a custom instruction set.

Current opcodes:

```
NOP   0x80

MOV   0x81
LOAD  0x82
STORE 0x83

ADD   0x84
SUB   0x85
INC   0x86
DEC   0x87

AND   0x88
OR    0x89
XOR   0x8A
NOT   0x8B

CMP   0x8C

JMP   0x8D
JZ    0x8E
JNZ   0x8F
JC    0x90
JNC   0x91

CALL  0x92
RET   0x93

PUSH  0x94
POP   0x95

IN    0x96
OUT   0x97

HALT  0x98

EI    0x99
DI    0x9A
RETI  0x9B
```

The user interface does not require the user to manually enter opcode values.

Built-in programs demonstrate the machine at a higher level.

---

# 6. Instruction Encoding

Instructions are decoded from raw bytes by the decoder.

Operands can be encoded as:

```
Register
Immediate
Address
```

The decoder validates operand types before producing an `Instruction`.

For example, a `MOV` destination must be a register.

A `LOAD` address must be an address operand.

A jump target must be an address operand.

This separates:

```
Raw machine bytes
        ↓
Instruction decoding
        ↓
Validated Instruction object
        ↓
Executor
```

This makes the CPU architecture easier to test and extend.

---

# 7. CPU Architecture

The CPU implementation is divided into several components.

## CPU

Responsible for:

- reset
- program loading
- fetch
- decode
- execute
- cycle progression
- exposing CPU state
- exposing memory

Conceptually:

```
Instruction CPU::cycle();
```

performs one complete CPU cycle.

---

## Decoder

The decoder converts raw memory bytes into structured instructions.

```
Memory bytes
     ↓
Decoder
     ↓
DecodeResult
     ├── Instruction
     └── bytes_consumed
```

Invalid instructions generate execution errors instead of silently executing unknown data.

---

## Executor

The executor performs the actual operation described by an instruction.

Responsibilities include:

- register operations
- arithmetic
- logical operations
- memory access
- stack operations
- branches
- calls/returns
- GPIO/UART/timer I/O
- halt behavior

This keeps instruction semantics separate from fetching and decoding.

---

# 8. Memory

The memory subsystem represents the CPU's 64 KiB address space.

Responsibilities:

- reset memory
- read byte
- write byte
- load program
- expose memory view

Memory also participates in memory-mapped I/O.

Conceptually:

```
CPU
 │
 ▼
Memory
 │
 ├── Normal RAM
 │
 └── Device Bus
       ├── GPIO
       ├── UART
       └── Timer
```

This is one of the important architectural features of MicroLink.

The CPU does not need a completely separate mechanism for talking to hardware.

---

# 9. Device Bus

The `DeviceBus` connects CPU memory accesses to peripherals.

The bus determines whether an address belongs to a device.

Example:

```
CPU writes 0x55 to 0xF000
             │
             ▼
        DeviceBus
             │
             ▼
           GPIO
             │
             ▼
        GPIO data = 0x55
```

The current bus exposes:

```
GPIO
UART
Timer
```

and provides:

```
handles(address)
read(address)
write(address, value)
tick()
```

This provides a clean hardware abstraction boundary.

---

# 10. GPIO

GPIO is implemented as an 8-bit digital port.

Registers:

```
0xF000  DATA
0xF001  DIRECTION
```

The frontend visualizes the GPIO data as eight LEDs.

For example:

```
10100101
```

is displayed as eight individual LED states.

The frontend also supports custom GPIO patterns.

Users can enter:

```
10100101
```

and load the corresponding CPU program.

This demonstrates:

```
CPU
 ↓
OUT instruction
 ↓
Memory-mapped GPIO
 ↓
DeviceBus
 ↓
LED visualization
```

---

# 11. UART

The UART provides a simple serial-output device.

The frontend displays UART output in a terminal-style panel.

Example:

```
Hello!
```

or:

```
MicroLink
```

The CPU generates the output through memory-mapped I/O.

This provides a simple demonstration of how embedded software can communicate with external hardware.

---

# 12. Timer

MicroLink includes a basic hardware timer.

Registers:

```
0xF020  Counter
0xF021  Compare
0xF022  Control
```

The timer contains:

- counter
- compare value
- enable state
- interrupt-pending state

The timer increments when enabled.

When the counter reaches the compare value:

```
counter == compare
```

the timer raises its pending event state.

The frontend visualizes:

- counter
- compare value
- enabled state
- interrupt state
- progress bar

The timer is intentionally simple in V1.

---

# 13. Runtime Layer

The runtime wraps the CPU and provides execution management.

It handles:

- cycle counting
- execution tracing
- step execution
- bounded run execution
- reset
- program loading

A runtime execution cycle records information such as:

```
cycle
PC before
PC after
Accumulator before
Accumulator after
Stack pointer
Instruction
```

This creates the execution trace used by the frontend.

---

# 14. Controller

The controller sits above the runtime.

It manages execution state:

```
Ready
Running
Paused
Halted
```

It exposes operations such as:

```
reset()
load()
step()
run()
pause()
```

This keeps execution policy outside the CPU itself.

---

# 15. Application Layer

The application layer provides the interface between the runtime and API.

It exposes:

```
reset
load_program
step
run
pause
state
cpu_state
trace
device state
```

The goal is to keep the REST server independent of the CPU implementation details.

---

# 16. REST API

The backend uses Crow.

The server runs by default at:

```
http://localhost:18080
```

Current API endpoints:

```
GET  /api/state
GET  /api/trace

POST /api/load
POST /api/step
POST /api/run
POST /api/reset
```

The API exposes CPU and peripheral state.

---

# 17. `/api/state`

Returns the current machine state.

Example:

```
{
  "pc": 4096,
  "accumulator": 0,
  "stack_pointer": 0,
  "instruction_register": 128,
  "halted": false,

  "gpio": 0,
  "gpio_direction": 0,

  "uart": "",

  "timer": 0,
  "timer_compare": 0,
  "timer_enabled": false,
  "timer_interrupt_pending": false
}
```

This endpoint powers most of the frontend dashboard.

---

# 18. `/api/load`

Loads a program into memory.

Example:

```
{
  "start_address": 4096,
  "program": [
    128,
    128,
    152
  ]
}
```

The program is loaded at:

```
0x1000
```

and the CPU program counter is set to the requested start address.

---

# 19. `/api/step`

Executes exactly one CPU instruction.

Example response:

```
{
  "pc": 4097,
  "cycles": 1
}
```

The frontend uses this endpoint for the debugger's STEP button.

---

# 20. `/api/run`

Executes up to a specified cycle limit.

Example request:

```
{
  "cycle_limit": 1000
}
```

The backend returns the resulting CPU state and execution information.

The cycle limit prevents accidental infinite execution from locking the server.

---

# 21. `/api/reset`

Resets the simulated machine.

Example:

```
POST /api/reset
```

Reset clears the CPU/runtime/peripheral state.

---

# 22. `/api/trace`

Returns the execution trace.

Example:

```
{
  "count": 1,
  "entries": [
    {
      "cycle": 1,
      "pc_before": 4096,
      "pc_after": 4097,
      "accumulator_before": 0,
      "accumulator_after": 0,
      "opcode": 128
    }
  ]
}
```

This allows the frontend to display the machine executing instruction by instruction.

---

# 23. Frontend

The frontend is implemented with React.

It provides a visual representation of the simulated computer.

The dashboard contains:

```
CPU
Registers
Execution Trace
GPIO LEDs
UART terminal
Timer
Memory
Execution controls
Program selection
```

The frontend communicates exclusively with the REST API.

---

# 24. Frontend Demonstrations

V1 contains several built-in demonstrations.

## NOP / HALT

Demonstrates basic instruction execution.

```
NOP
NOP
NOP
NOP
HALT
```

Useful for testing:

- load
- step
- run
- trace
- halt detection

---

## LED Counter

Demonstrates CPU → GPIO communication.

The CPU writes different values to the GPIO device.

The frontend converts the GPIO byte into eight LEDs.

---

## GPIO Pattern

Demonstrates a visual binary pattern.

The frontend also allows custom patterns such as:

```
00000001
10101010
11110000
10100101
11111111
```

---

## UART Hello

Demonstrates CPU → UART communication.

Characters are written through the UART device and appear in the terminal.

---

## UART MicroLink

Demonstrates a longer serial message.

This makes the UART feel more like an actual embedded development environment rather than a single-character test.

---

## Timer Demo

Demonstrates:

```
CPU
 ↓
Timer configuration
 ↓
Timer counting
 ↓
Compare event
 ↓
Frontend visualization
```

The timer can be observed through its counter, compare value, enabled state and interrupt-pending state.

---

# 25. Frontend Execution Modes

The UI provides three useful execution modes.

## STEP

Executes exactly one instruction.

Useful for debugging the architecture.

---

## RUN

Executes up to the configured backend cycle limit.

Useful for normal program execution.

---

## LIVE RUN

Executes one instruction at a time with a visible delay.

This is particularly useful for:

- watching LEDs change
- observing the timer
- following the program counter
- watching the trace grow
- understanding the CPU cycle

The live speed can be adjusted from the UI.

---

# 26. Memory Viewer

The frontend includes a memory viewer.

It displays:

```
ADDRESS    BYTE
0x1000     0x80
0x1001     0x81
0x1002     0x00
...
```

The memory view is scrollable so the user can inspect larger sections of the address space.

This makes the relationship between program bytes and CPU execution visible.

---

# 27. Project Structure

The project is organized into layers.

A simplified structure is:

```
Microlink/
│
├── CMakeLists.txt
├── README.md
│
├── include/
│   └── microlink/
│       │
│       ├── cpu/
│       │   ├── cpu.hpp
│       │   ├── cpu_state.hpp
│       │   ├── decoder.hpp
│       │   ├── executor.hpp
│       │   ├── instruction.hpp
│       │   ├── memory.hpp
│       │   ├── opcode.hpp
│       │   ├── register_id.hpp
│       │   └── types.hpp
│       │
│       ├── device/
│       │   ├── device.hpp
│       │   ├── device_bus.hpp
│       │   ├── gpio.hpp
│       │   ├── uart.hpp
│       │   └── timer.hpp
│       │
│       ├── runtime/
│       │   ├── runtime.hpp
│       │   ├── controller.hpp
│       │   └── trace.hpp
│       │
│       ├── application/
│       │   └── application.hpp
│       │
│       └── api/
│           └── server.hpp
│
├── src/
│   └── microlink/
│       │
│       ├── cpu/
│       │   ├── cpu.cpp
│       │   ├── decoder.cpp
│       │   ├── executor.cpp
│       │   └── memory.cpp
│       │
│       ├── device/
│       │   ├── device_bus.cpp
│       │   ├── gpio.cpp
│       │   ├── uart.cpp
│       │   └── timer.cpp
│       │
│       ├── runtime/
│       │   ├── runtime.cpp
│       │   └── controller.cpp
│       │
│       ├── application/
│       │   └── application.cpp
│       │
│       └── api/
│           └── server.cpp
│
├── tests/
│   ├── cpu/
│   ├── runtime/
│   ├── application/
│   └── api/
│
├── frontend/
│   └── ...
│
└── build/
```

The exact directory contents may evolve as the project grows.

---

# 28. Layering

MicroLink follows a layered architecture.

```
┌──────────────────────────────┐
│          React UI            │
└──────────────┬───────────────┘
               │ HTTP/JSON
┌──────────────▼───────────────┐
│        Crow REST API         │
└──────────────┬───────────────┘
               │
┌──────────────▼───────────────┐
│       Application Layer      │
└──────────────┬───────────────┘
               │
┌──────────────▼───────────────┐
│ Runtime / Controller         │
└──────────────┬───────────────┘
               │
┌──────────────▼───────────────┐
│             CPU              │
├──────────────┬───────────────┤
│ Decoder      │ Executor      │
├──────────────┴───────────────┤
│ CPU State / Memory           │
└──────────────┬───────────────┘
               │
┌──────────────▼───────────────┐
│         Device Bus           │
├──────────┬──────────┬────────┤
│  GPIO    │   UART   │ Timer  │
└──────────┴──────────┴────────┘
```

Each layer has a specific responsibility.

---

# 29. Build Requirements

## Operating System

The project has primarily been developed and tested on:

```
Windows
```

with:

```
MSYS2
UCRT64
```

---

## Required C++ Toolchain

Recommended:

```
MSYS2 UCRT64
GCC
CMake
Ninja
```

The compiler should support modern C++ features including:

- `std::span`
- `std::array`
- `std::vector`
- modern enum/class syntax
- C++20 language features

---

# 30. Backend Dependencies

The backend uses:

```
CMake
Crow
GoogleTest
```

Crow is used for the HTTP server.

GoogleTest is used for automated tests.

Dependencies are handled through the CMake configuration.

---

# 31. Frontend Requirements

The frontend requires:

```
Node.js
npm
React
Vite
```

Install dependencies from the frontend directory:

```
npm install
```

---

# 32. Building the Backend

From the project root:

```
cmake -S . -B build -G Ninja
```

Then:

```
cmake --build build
```

The server executable is generated as:

```
build/microlink_server.exe
```

---

# 33. Running the Backend

From the project root:

```
./build/microlink_server.exe
```

The server should report something similar to:

```
Crow/master server is running at
http://0.0.0.0:18080
```

The REST API is then available at:

```
http://localhost:18080
```

---

# 34. Running the Frontend

Open another terminal.

Go to the frontend directory:

```
cd frontend
```

Install dependencies if necessary:

```
npm install
```

Start the development server:

```
npm run dev
```

Vite normally provides an address similar to:

```
http://localhost:5173
```

Open that address in a browser.

The frontend communicates with:

```
http://localhost:18080
```

---

# 35. Basic Verification

After starting the backend, verify the API.

PowerShell:

```
Invoke-WebRequest `
  -Method GET `
  -Uri "http://localhost:18080/api/state" `
  -UseBasicParsing
```

The response should contain CPU/device state.

---

# 36. Loading a Program Manually

PowerShell example:

```
Invoke-WebRequest `
  -Method POST `
  -Uri "http://localhost:18080/api/load" `
  -ContentType "application/json" `
  -Body '{"start_address":4096,"program":[128,152]}' `
  -UseBasicParsing
```

This loads:

```
NOP
HALT
```

at:

```
0x1000
```

---

# 37. Testing STEP

After loading:

```
Invoke-WebRequest `
  -Method POST `
  -Uri "http://localhost:18080/api/step" `
  -UseBasicParsing
```

Then:

```
Invoke-WebRequest `
  -Method GET `
  -Uri "http://localhost:18080/api/state" `
  -UseBasicParsing
```

The program counter should advance.

The trace can be checked with:

```
Invoke-WebRequest `
  -Method GET `
  -Uri "http://localhost:18080/api/trace" `
  -UseBasicParsing
```

---

# 38. Testing RUN

Example:

```
Invoke-WebRequest `
  -Method POST `
  -Uri "http://localhost:18080/api/run" `
  -ContentType "application/json" `
  -Body '{"cycle_limit":100}' `
  -UseBasicParsing
```

For a program containing `HALT`, execution should stop when the CPU reaches the halt instruction.

---

# 39. Testing RESET

```
Invoke-WebRequest `
  -Method POST `
  -Uri "http://localhost:18080/api/reset" `
  -UseBasicParsing
```

The CPU and runtime state should return to the reset condition.

---

# 40. Running Tests

Build the project:

```
cmake --build build
```

Then run:

```
ctest --test-dir build --output-on-failure
```

The test suite covers components including:

```
CPU
Instruction
Decoder
Executor
Memory
Runtime
Trace
Controller
Application
API
```

A successful build/test cycle should finish without linker or test failures.

---

# 41. Testing Philosophy

MicroLink uses unit tests to validate individual architectural layers.

For example:

```
Memory tests
    ↓
CPU tests
    ↓
Executor tests
    ↓
Runtime tests
    ↓
Application tests
    ↓
API tests
```

This allows failures to be localized.

A CPU bug should not require debugging the React application first.

---

# 42. Common Development Problems

## Crow FetchContent / Git Error

If CMake reports something similar to:

```
Failed to get the hash for HEAD

fatal: not a git repository
```

the CMake FetchContent dependency directory may be corrupted.

A clean rebuild of the dependency/build directory may be required.

For example:

```
rm -rf build
```

or remove the build directory using Windows Explorer/PowerShell.

Then configure again:

```
cmake -S . -B build -G Ninja
cmake --build build
```

---

# 43. PowerShell and curl

On Windows PowerShell, `curl` may resolve to PowerShell's `Invoke-WebRequest`.

Therefore this:

```
curl -X POST ...
```

may not behave like Linux curl.

For MicroLink development, use:

```
Invoke-WebRequest
```

or explicitly:

```
curl.exe
```

The recommended PowerShell style is:

```
Invoke-WebRequest `
  -Method POST `
  -Uri "http://localhost:18080/api/step" `
  -UseBasicParsing
```

---

# 44. Design Principles

MicroLink V1 follows several design principles.

## Separation of concerns

The CPU should not know about React.

The frontend should not know how instructions are implemented.

The REST API should not implement CPU behavior.

---

## Explicit interfaces

Subsystems communicate through well-defined interfaces.

Examples:

```
CPU → Memory
CPU → Executor
Memory → DeviceBus
Runtime → CPU
Application → Runtime
API → Application
React → API
```

---

## Testability

Important behavior should be testable without starting the entire application.

For example:

```
Executor tests
```

should not require:

```
Crow
React
Browser
```

---

## Observability

The machine should be observable while executing.

This is why V1 includes:

- CPU state
- execution trace
- memory viewer
- GPIO visualization
- UART terminal
- timer visualization

---

# 45. What Makes This an Embedded Computer?

MicroLink is more than an instruction interpreter because it combines:

```
CPU
+
Memory
+
Instruction Set
+
Memory-Mapped I/O
+
Peripheral Devices
+
Device Bus
+
Runtime
+
Execution Control
+
Serial Interface
+
GPIO
+
Timer
+
Debugger
+
Visualization
```

This models several important concepts found in real embedded systems.

The architecture is intentionally simplified, but the architectural ideas are real.

---

# 46. V1 Limitations

MicroLink V1 intentionally does not attempt to reproduce every feature of a commercial microcontroller.

Current limitations include:

- simplified CPU architecture
- simplified interrupt behavior
- limited peripheral functionality
- no physical hardware
- no real electrical timing
- no DMA
- no ADC
- no SPI
- no I2C
- no PWM
- no bootloader
- no assembler
- no compiler toolchain
- no debugger protocol such as GDB
- no cycle-accurate hardware timing
- limited flag visualization
- limited instruction inspection in the UI

These are deliberate scope boundaries for V1.

---

# 47. Future Roadmap

Future development can be divided into versions.

## V2 — Better CPU Architecture

Potential additions:

- complete status flags
- more precise flag behavior
- better interrupt handling
- interrupt vector table
- improved stack behavior
- instruction timing
- clearer register model
- assembler syntax

---

## V3 — Developer Toolchain

Potential additions:

```
MicroLink Assembly
        ↓
Assembler
        ↓
Machine Code
        ↓
MicroLink CPU
```

Features could include:

- `.asm` source files
- labels
- symbolic addresses
- constants
- assembler diagnostics
- downloadable binaries

---

## V4 — More Peripherals

Potential devices:

```
SPI
I2C
PWM
ADC
GPIO interrupts
watchdog timer
RTC
additional UART
```

---

## V5 — Interrupt Architecture

Potential improvements:

- interrupt vector table
- interrupt priorities
- interrupt masking
- external interrupts
- timer interrupts
- UART interrupts
- GPIO interrupts
- proper `EI`
- `DI`
- `RETI`

---

## V6 — Debugger

Potential debugger functionality:

```
breakpoints
watchpoints
register editing
memory editing
instruction disassembly
call stack
conditional breakpoints
execution history
```

---

## V7 — Hardware

The architecture could eventually be mapped onto physical hardware.

Potential platforms:

```
FPGA
custom PCB
microcontroller implementation
```

The simulator could then become a reference model for a real implementation.

---

# 48. Possible Long-Term Architecture

The long-term goal could look like:

```
                    MicroLink Toolchain
                           │
                    ┌──────▼──────┐
                    │   Assembler │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │   Binary    │
                    └──────┬──────┘
                           │
             ┌─────────────▼─────────────┐
             │       MicroLink CPU       │
             └─────────────┬─────────────┘
                           │
                     Device Bus
          ┌────────┬───────┼───────┬────────┐
          ▼        ▼       ▼       ▼        ▼
        GPIO      UART   Timer    SPI      I2C
          │        │       │
          └────────┴───────┴───────────────┐
                                           ▼
                                  Debugger / UI
```

This would transform MicroLink from a simulator into a small complete embedded development platform.

---

# 49. Interview / Portfolio Value

MicroLink demonstrates knowledge across multiple engineering domains.

## C++

Demonstrates:

- classes
- interfaces
- RAII
- STL containers
- `std::span`
- enums
- namespaces
- const correctness
- memory handling
- modular architecture

## Computer Architecture

Demonstrates:

- instruction sets
- registers
- program counter
- instruction decoding
- execution
- stack
- memory
- branching
- CPU cycles

## Embedded Systems

Demonstrates:

- memory-mapped I/O
- GPIO
- UART
- timers
- device buses
- interrupts concept
- hardware abstraction

## Software Architecture

Demonstrates:

- layered architecture
- separation of concerns
- runtime/controller design
- API boundaries
- testable components

## Backend

Demonstrates:

- HTTP
- JSON
- REST APIs
- C++ server development
- CORS

## Frontend

Demonstrates:

- React
- asynchronous API communication
- real-time state visualization
- debugging interfaces
- hardware visualization

## Testing

Demonstrates:

- GoogleTest
- unit testing
- subsystem testing
- API testing
- build/test automation

---

# 50. Example Portfolio Description

A concise CV description could be:

> **MicroLink — Custom 8-bit Embedded Computer Simulator**
>
> Designed and implemented a custom 8-bit CPU and 16-bit/64 KiB memory architecture in modern C++, including instruction decoding/execution, stack operations, branching, memory-mapped I/O, GPIO, UART and hardware timer peripherals. Built a runtime/controller layer with cycle-level execution tracing and exposed the simulated machine through a Crow REST API and React-based hardware debugger with live CPU, memory, GPIO, UART and timer visualization. Added automated GoogleTest coverage across CPU, runtime, application and API layers.

---

# 51. Current V1 Architecture Summary

```
                    ┌─────────────────────┐
                    │      React UI       │
                    │                     │
                    │ CPU / Trace / GPIO  │
                    │ UART / Timer / RAM  │
                    └──────────┬──────────┘
                               │
                              HTTP
                               │
                    ┌──────────▼──────────┐
                    │      Crow API       │
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │    Application      │
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │ Runtime / Controller│
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │        CPU          │
                    │                     │
                    │ Fetch               │
                    │ Decode              │
                    │ Execute             │
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │       Memory        │
                    │      64 KiB         │
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │     Device Bus      │
                    └────┬──────┬──────┬──┘
                         │      │      │
                       GPIO    UART   Timer
                         │      │      │
                         ▼      ▼      ▼
                       LEDs  Terminal Counter
```

---

# 52. V1 Definition of Done

MicroLink V1 is considered complete when the following remain functional:

-  CPU reset
-  Program loading
-  Instruction fetch
-  Instruction decoding
-  Instruction execution
-  Memory
-  Stack
-  Arithmetic/logic
-  Branching
-  GPIO
-  UART
-  Timer
-  Device bus
-  Runtime
-  Execution trace
-  Step execution
-  Bounded run
-  REST API
-  React frontend
-  LED visualization
-  UART terminal
-  Timer visualization
-  Memory visualization
-  Custom GPIO pattern
-  Built-in demonstration programs
-  Automated tests

---

# 53. Development Workflow

Typical development cycle:

```
1. Modify C++ source
        ↓
2. Build
        ↓
3. Run tests
        ↓
4. Start server
        ↓
5. Start frontend
        ↓
6. Test through UI
        ↓
7. Inspect API / trace
        ↓
8. Repeat
```

Backend:

```
cmake --build build
ctest --test-dir build --output-on-failure
./build/microlink_server.exe
```

Frontend:

```
cd frontend
npm run dev
```

---

# 54. Philosophy

MicroLink is intentionally built incrementally.

The goal is not to immediately implement every feature found in a modern microcontroller.

Instead:

```
CPU
 ↓
Memory
 ↓
Instructions
 ↓
Runtime
 ↓
I/O
 ↓
Peripherals
 ↓
API
 ↓
Visualization
 ↓
Toolchain
 ↓
Hardware
```

Each layer provides a foundation for the next.

This makes the project understandable, testable and extensible.

---

# 55. Final Project Summary

MicroLink V1 is a functional custom embedded-computer simulation platform.

It combines:

```
Custom ISA
+
8-bit CPU
+
64 KiB memory
+
Instruction decoder
+
Instruction executor
+
Stack
+
Runtime
+
Execution tracing
+
Device bus
+
GPIO
+
UART
+
Timer
+
REST API
+
React debugger
+
Automated tests
```

The project provides a practical demonstration of how software, computer architecture, embedded systems and visualization can be combined into one coherent engineering project.

V1 establishes the core platform.

Future versions can extend the architecture with a richer instruction set, proper interrupt architecture, assembler/toolchain support, additional peripherals, debugger functionality and eventually physical hardware.

---


---

## Author
Anubhab Bhattacharya

**MicroLink**

Custom 8-bit embedded computer simulator built from scratch.

```
```
