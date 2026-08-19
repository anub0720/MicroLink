#include <gtest/gtest.h>

#include <stdexcept>

#include "microlink/cpu/executor.hpp"

using namespace microlink::cpu;

TEST(Executor, MovesRegisterToRegister) {
    CPUState state;
    Memory memory;

    state.set_register(1, 42);

    const Instruction instruction(
        Opcode::MOV,
        Operand::reg(RegisterId::A),
        Operand::reg(RegisterId::R1)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.accumulator(), 42);
}

TEST(Executor, MovesAccumulatorToRegister) {
    CPUState state;
    Memory memory;

    state.set_accumulator(73);

    const Instruction instruction(
        Opcode::MOV,
        Operand::reg(RegisterId::R3),
        Operand::reg(RegisterId::A)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.register_value(3), 73);
}

TEST(Executor, MovesImmediateToAccumulator) {
    CPUState state;
    Memory memory;

    const Instruction instruction(
        Opcode::MOV,
        Operand::reg(RegisterId::A),
        Operand::immediate(42)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.accumulator(), 42);
}

TEST(Executor, AddsRegisterToAccumulator) {
    CPUState state;
    Memory memory;

    state.set_accumulator(10);
    state.set_register(1, 5);

    const Instruction instruction(
        Opcode::ADD,
        Operand::reg(RegisterId::R1)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.accumulator(), 15);
}

TEST(Executor, AdditionUpdatesCarry) {
    CPUState state;
    Memory memory;

    state.set_accumulator(0xFF);
    state.set_register(0, 1);

    const Instruction instruction(
        Opcode::ADD,
        Operand::reg(RegisterId::R0)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.accumulator(), 0);
    EXPECT_TRUE(state.flags().carry());
    EXPECT_TRUE(state.flags().zero());
}

TEST(Executor, SubtractsRegisterFromAccumulator) {
    CPUState state;
    Memory memory;

    state.set_accumulator(10);
    state.set_register(2, 4);

    const Instruction instruction(
        Opcode::SUB,
        Operand::reg(RegisterId::R2)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.accumulator(), 6);
}

TEST(Executor, CompareDoesNotModifyAccumulator) {
    CPUState state;
    Memory memory;

    state.set_accumulator(10);
    state.set_register(1, 10);

    const Instruction instruction(
        Opcode::CMP,
        Operand::reg(RegisterId::R1)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.accumulator(), 10);
    EXPECT_TRUE(state.flags().zero());
}

TEST(Executor, LoadsByteFromMemoryIntoRegister) {
    CPUState state;
    Memory memory;

    memory.write(0x2040, 0xAB);

    const Instruction instruction(
        Opcode::LOAD,
        Operand::reg(RegisterId::R2),
        Operand::address(0x2040)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.register_value(2), 0xAB);
}

TEST(Executor, StoresRegisterIntoMemory) {
    CPUState state;
    Memory memory;

    state.set_register(3, 0xCD);

    const Instruction instruction(
        Opcode::STORE,
        Operand::reg(RegisterId::R3),
        Operand::address(0x4000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(memory.read(0x4000), 0xCD);
}

TEST(Executor, JumpAlwaysChangesProgramCounter) {
    CPUState state;
    Memory memory;

    state.set_program_counter(0x1000);

    const Instruction instruction(
        Opcode::JMP,
        Operand::address(0x3000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.program_counter(), 0x3000);
}

TEST(Executor, JumpIfZeroTaken) {
    CPUState state;
    Memory memory;

    state.flags().set_zero(true);

    const Instruction instruction(
        Opcode::JZ,
        Operand::address(0x4000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.program_counter(), 0x4000);
}

TEST(Executor, JumpIfZeroNotTaken) {
    CPUState state;
    Memory memory;

    state.set_program_counter(0x1234);
    state.flags().set_zero(false);

    const Instruction instruction(
        Opcode::JZ,
        Operand::address(0x4000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.program_counter(), 0x1234);
}

TEST(Executor, JumpIfNotZeroTaken) {
    CPUState state;
    Memory memory;

    state.flags().set_zero(false);

    const Instruction instruction(
        Opcode::JNZ,
        Operand::address(0x5000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.program_counter(), 0x5000);
}

TEST(Executor, JumpIfNotZeroNotTaken) {
    CPUState state;
    Memory memory;

    state.set_program_counter(0x1234);
    state.flags().set_zero(true);

    const Instruction instruction(
        Opcode::JNZ,
        Operand::address(0x5000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.program_counter(), 0x1234);
}

TEST(Executor, JumpIfCarryTaken) {
    CPUState state;
    Memory memory;

    state.flags().set_carry(true);

    const Instruction instruction(
        Opcode::JC,
        Operand::address(0x6000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.program_counter(), 0x6000);
}

TEST(Executor, JumpIfCarryNotTaken) {
    CPUState state;
    Memory memory;

    state.set_program_counter(0x1234);
    state.flags().set_carry(false);

    const Instruction instruction(
        Opcode::JC,
        Operand::address(0x6000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.program_counter(), 0x1234);
}

TEST(Executor, JumpIfNotCarryTaken) {
    CPUState state;
    Memory memory;

    state.flags().set_carry(false);

    const Instruction instruction(
        Opcode::JNC,
        Operand::address(0x7000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.program_counter(), 0x7000);
}

TEST(Executor, JumpIfNotCarryNotTaken) {
    CPUState state;
    Memory memory;

    state.set_program_counter(0x1234);
    state.flags().set_carry(true);

    const Instruction instruction(
        Opcode::JNC,
        Operand::address(0x7000)
    );

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.program_counter(), 0x1234);
}

TEST(Executor, NopDoesNotChangeState) {
    CPUState state;
    Memory memory;

    state.set_accumulator(42);
    state.set_program_counter(0x1234);

    const Instruction instruction(Opcode::NOP);

    Executor::execute(instruction, state, memory, 0x1234);

    EXPECT_EQ(state.accumulator(), 42);
    EXPECT_EQ(state.program_counter(), 0x1234);
}

TEST(Executor, RejectsInvalidBranchOperand) {
    CPUState state;
    Memory memory;

    const Instruction instruction(
        Opcode::JMP,
        Operand::immediate(10)
    );

    EXPECT_THROW(
        Executor::execute(instruction, state, memory,0x1234),
        std::runtime_error
    );
}
TEST(Executor, PushStoresRegisterAndDecrementsStackPointer) {
    CPUState state;
    Memory memory;

    state.set_stack_pointer(0x8000);
    state.set_register(2, 0xAB);

    const Instruction instruction(
        Opcode::PUSH,
        Operand::reg(RegisterId::R2)
    );

    Executor::execute(
        instruction,
        state,
        memory,
        0x1234
    );

    EXPECT_EQ(state.stack_pointer(), 0x7FFF);
    EXPECT_EQ(memory.read(0x7FFF), 0xAB);
}

TEST(Executor, PopRestoresRegisterAndIncrementsStackPointer) {
    CPUState state;
    Memory memory;

    state.set_stack_pointer(0x7FFF);
    memory.write(0x7FFF, 0xCD);

    const Instruction instruction(
        Opcode::POP,
        Operand::reg(RegisterId::R2)
    );

    Executor::execute(
        instruction,
        state,
        memory,
        0x1234
    );

    EXPECT_EQ(state.register_value(2), 0xCD);
    EXPECT_EQ(state.stack_pointer(), 0x8000);
}

TEST(Executor, CallPushesReturnAddressAndJumps) {
    CPUState state;
    Memory memory;

    state.set_stack_pointer(0x8000);

    const Instruction instruction(
        Opcode::CALL,
        Operand::address(0x3000)
    );

    Executor::execute(
        instruction,
        state,
        memory,
        0x1234
    );

    EXPECT_EQ(state.program_counter(), 0x3000);
    EXPECT_EQ(state.stack_pointer(), 0x7FFE);

    EXPECT_EQ(memory.read(0x7FFE), 0x34);
    EXPECT_EQ(memory.read(0x7FFF), 0x12);
}

TEST(Executor, RetPopsReturnAddress) {
    CPUState state;
    Memory memory;

    state.set_stack_pointer(0x7FFE);

    memory.write(0x7FFE, 0x34);
    memory.write(0x7FFF, 0x12);

    const Instruction instruction(Opcode::RET);

    Executor::execute(
        instruction,
        state,
        memory,
        0x9999
    );

    EXPECT_EQ(state.program_counter(), 0x1234);
    EXPECT_EQ(state.stack_pointer(), 0x8000);
}