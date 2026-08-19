# MicroLink — Machine Specification v1.0

## 1. Project Identity

MicroLink is an interactive, software-defined, 8-bit embedded computer model inspired by concepts from the 8085 microprocessor and the accompanying Microprocessor course material.

MicroLink is NOT intended to be a cycle-accurate 8085 emulator.

The 8085 is used as a conceptual foundation for:
- processor organization
- registers and ALU
- memory organization
- address/data/control buses
- memory and I/O interfacing
- interrupts
- peripheral interfacing
- serial communication
- basic microcomputer operating-system concepts

The project will implement these concepts as a coherent, observable computer system using modern C++.

---

## 2. Design Philosophy

### 2.1 Conceptual, not hardware-accurate

The system models the behavior and relationships that are useful for understanding an embedded computer.

It does not attempt to model:
- transistor/electrical behavior
- physical signal timing
- exact hardware electrical characteristics
- a complete real MCU
- complete 8085 compatibility

### 2.2 Visual-first

The primary product is an interactive visualizer.

Users should be able to understand the system without knowing the MicroLink instruction set or assembly language.

### 2.3 Progressive disclosure

System behavior is presented at multiple levels:

1. High-level system log
2. Architectural/event details
3. Component details
4. CPU/instruction details
5. Machine-cycle/T-state details where appropriate

The user can start with a simple explanation and progressively inspect deeper implementation details.

### 2.4 Separation of concerns

The simulation core must be independent of the GUI.

The GUI observes and controls the simulation through well-defined interfaces. It must not directly manipulate internal CPU/peripheral state.

---

## 3. Machine Model

### 3.1 Data width

The conceptual machine has an 8-bit data path.

### 3.2 Address width

The conceptual machine uses a 16-bit address space.

Therefore the address space is:

64 KiB

### 3.3 Architecture style

The machine is 8085-inspired but not instruction-set compatible with the 8085.

The final MicroLink instruction set will be designed specifically to support the embedded-system demonstrations and CPU concepts required by the project.

---

## 4. CPU Concept

The CPU model will contain recognizable microprocessor concepts including:

- Accumulator
- General-purpose registers
- Program Counter (PC)
- Stack Pointer (SP)
- Instruction Register (IR)
- Flags
- ALU

The exact register organization and instruction set will be defined during M2 — CPU + Instruction Engine Design.

---

## 5. CPU Execution Model

The CPU will conceptually execute work through:

FETCH
→ DECODE
→ EXECUTE

The simulator may expose machine cycles and T-state-like timing concepts for visualization.

This timing model is conceptual and is not intended to claim physical timing accuracy.

---

## 6. Memory Model

The machine will provide an addressable memory subsystem.

Memory must support:
- reads
- writes
- address-based access
- program/data storage
- interaction with the system bus

The exact memory map will be finalized during detailed system design.

The visualizer will expose the memory map and relevant memory transactions.

---

## 7. System Bus

The system will conceptually model:

- Address bus
- Data bus
- Control signals

A bus transaction must contain enough information to represent:
- source
- target
- address
- operation
- data
- relevant control information

The bus is the conceptual communication mechanism between CPU, memory and peripherals.

---

## 8. Address Decoding

The system will contain an address-decoding mechanism that determines which memory/peripheral region receives a transaction.

Conceptually:

CPU
→ Address/Data/Control
→ Bus
→ Address Decoder
→ Memory or Peripheral

The exact address map and decoding rules will be defined during detailed design.

---

## 9. I/O Model

MicroLink will support a conceptual I/O mechanism for communicating with peripherals.

The project will model peripheral access in a way that makes processor-to-peripheral communication observable.

The exact relationship between memory space and I/O space will be finalized during detailed design.

---

## 10. Peripheral Model

Peripherals are independent system components connected through the bus.

The initial peripheral set is inspired by peripherals covered in the course:

- 8255-inspired Programmable Peripheral Interface (PPI)
- 8253-inspired Timer/Counter
- 8251-inspired Serial Interface

These are conceptual implementations, not claims of exact hardware emulation.

---

## 11. PPI Model

The PPI will conceptually contain:

- Port A
- Port B
- Port C
- Control register

It will support the relevant programmable I/O concepts required for project demonstrations.

The exact register layout, modes, and behavior will be defined during peripheral design.

---

## 12. Timer Model

The timer will contain a configurable counter and timing behavior.

Conceptually:

Timer
→ counter/event condition
→ event
→ optional interrupt request

The timer will operate according to simulation time rather than real-time sleeping.

---

## 13. Serial Model

The serial peripheral will be inspired by the 8251 USART concepts covered in the course.

It will conceptually provide:
- transmit
- receive
- status
- control
- data-ready events
- optional interrupt generation

The project will model communication behavior rather than physical serial signaling.

---

## 14. Interrupt System

The system will contain an explicit interrupt subsystem.

Peripherals may generate interrupt requests.

The conceptual flow is:

Peripheral
→ Interrupt Request
→ Interrupt System
→ CPU
→ Acknowledge
→ Context/return-state handling
→ ISR
→ Return
→ Resume execution

The interrupt system will support the concepts of:
- pending interrupts
- interrupt enable/disable
- interrupt priority
- interrupt acknowledgement
- ISR entry
- ISR return

Exact interrupt-vector and priority mechanisms will be defined during interrupt-system design.

---

## 15. Runtime / Mini-OS

MicroLink will contain a small runtime inspired by the microcomputer OS concepts in the course.

It is NOT intended to be a full operating system or RTOS.

The runtime may provide:
- task/process abstraction
- task states
- basic scheduling
- I/O waiting
- resource accounting
- error handling

Initial task states:

- READY
- RUNNING
- WAITING
- COMPLETED

The exact scheduling model will be defined during runtime design.

---

## 16. Simulation Time

Simulation time is independent from real wall-clock time.

The simulation will use a deterministic conceptual clock/event mechanism.

This allows:
- reproducible execution
- fast automated testing
- pause/resume
- single-step execution
- replay
- deterministic demonstrations

The GUI may animate simulation events at a convenient visual speed without changing the underlying simulation semantics.

---

## 17. Event and Trace Model

The simulation core will produce structured events representing significant system activity.

Potential events include:

- CPU_FETCH
- MEMORY_READ
- MEMORY_WRITE
- IO_READ
- IO_WRITE
- TIMER_EVENT
- INTERRUPT_REQUEST
- INTERRUPT_ACK
- ISR_ENTER
- ISR_EXIT
- SERIAL_TX
- SERIAL_RX
- TASK_SWITCH

The final event schema will be defined during event/trace design.

Events will support progressive inspection from high-level descriptions to deeper CPU/machine details.

---

## 18. Visualizer

The final application will provide an interactive graphical interface.

The visualizer should expose, as appropriate:

- system architecture
- CPU state
- memory
- bus activity
- peripherals
- interrupts
- runtime/task state
- event trace
- execution timeline

Primary controls should include:

- Run
- Pause
- Reset
- Step
- Trigger external event
- Select demonstration

The UI must not require users to know the MicroLink instruction set.

---

## 19. Demonstration Philosophy

The system must provide predefined demonstrations that show complete conceptual flows.

Examples:

### CPU ↔ Memory

CPU
→ Bus
→ Memory
→ Data
→ CPU

### Programmed I/O

CPU
→ Bus
→ PPI
→ Virtual external device

### Interrupt-driven I/O

Peripheral
→ IRQ
→ CPU
→ ISR
→ Peripheral servicing
→ Resume

### Timer Interrupt

Timer
→ Event
→ IRQ
→ CPU
→ ISR
→ Resume

### Serial Receive

Virtual sender
→ Serial
→ Data Ready
→ IRQ
→ CPU
→ ISR
→ Application

### Basic Multiprogramming

Task A
→ waits for I/O
→ Task B executes
→ I/O interrupt
→ Task A becomes ready
→ scheduler
→ Task A resumes

---

## 20. Testing Principles

The simulation core must be testable independently of the GUI.

Testing will include:

- unit tests
- integration tests
- complete scenario tests
- invalid-operation tests
- deterministic/reproducibility tests

The GUI is a presentation and interaction layer; system correctness must primarily be verified at the simulation-core level.

---

## 21. Explicit Non-Goals

MicroLink will NOT attempt to provide:

- complete 8085 instruction-set compatibility
- a complete 8085 emulator
- transistor/electrical simulation
- physical hardware simulation
- real MCU execution
- real electrical I²C/SPI/BLE behavior
- a full Linux-like operating system
- a full RTOS
- physical power measurements
- physical hardware timing claims

Additional features such as DMA may be considered later as stretch goals.

---

## 22. Design Milestone Boundary

This document defines the agreed machine-level requirements.

The following details are intentionally deferred to later design milestones:

- exact register set
- exact instruction set
- exact instruction encoding
- exact memory map
- exact I/O map
- exact bus API
- exact peripheral register maps
- exact interrupt vector mechanism
- exact scheduler
- exact event schema
- exact C++ class/interface structure
- exact GUI framework and layout

These decisions must be made before implementation of their respective modules.

---

## 23. Current Status

M1 — Machine Model: COMPLETE (conceptual specification)

Next milestone:

M2 — CPU + Instruction Engine Design

M2 will define the CPU state, ALU behavior, registers, instruction abstraction, execution lifecycle, and the minimum MicroLink instruction set required to support the system demonstrations.
