#include <array>
#include <gtest/gtest.h>

#include "microlink/cpu/cpu.hpp"

using namespace microlink::cpu;

TEST(CPU, StartsWithResetState) {
    CPU cpu;

    EXPECT_EQ(cpu.state().program_counter(), 0);
    EXPECT_EQ(cpu.state().accumulator(), 0);
    EXPECT_FALSE(cpu.state().halted());
}

TEST(CPU, ResetRestoresArchitecturalState) {
    CPU cpu;

    cpu.state().set_accumulator(42);
    cpu.state().set_program_counter(0x1234);
    cpu.state().set_register(2, 99);

    cpu.reset();

    EXPECT_EQ(cpu.state().accumulator(), 0);
    EXPECT_EQ(cpu.state().program_counter(), 0);
    EXPECT_EQ(cpu.state().register_value(2), 0);
}

TEST(CPU, FetchesAndDecodesNop) {
    CPU cpu;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    cpu.load_program(program, 0x1000);

    const Instruction instruction =
        cpu.fetch_and_decode();

    EXPECT_EQ(instruction.opcode(), Opcode::NOP);
    EXPECT_EQ(
        cpu.state().instruction_register(),
        to_byte(Opcode::NOP)
    );
}

TEST(CPU, FetchDoesNotAdvanceProgramCounter) {
    CPU cpu;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    cpu.load_program(program, 0x1000);

    cpu.fetch_and_decode();

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x1000
    );
}

TEST(CPU, CycleAdvancesProgramCounterAfterNop) {
    CPU cpu;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    cpu.load_program(program, 0x1000);

    cpu.cycle();

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x1001
    );
}

TEST(CPU, FetchesAndDecodesLoadInstruction) {
    CPU cpu;

    const std::array<Byte, 6> program{
        to_byte(Opcode::LOAD),
        0x00,
        to_byte(RegisterId::R1),
        0x02,
        0x20,
        0x40
    };

    cpu.load_program(program, 0x1000);

    const Instruction instruction =
        cpu.fetch_and_decode();

    EXPECT_EQ(instruction.opcode(), Opcode::LOAD);
    EXPECT_EQ(
        instruction.operand(0).register_id(),
        RegisterId::R1
    );
    EXPECT_EQ(
        instruction.operand(1).type,
        OperandType::Address
    );
    EXPECT_EQ(
        instruction.operand(1).value,
        0x2040
    );

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x1000
    );
}

TEST(CPU, CycleAdvancesByLoadInstructionLength) {
    CPU cpu;

    const std::array<Byte, 6> program{
        to_byte(Opcode::LOAD),
        0x00,
        to_byte(RegisterId::R1),
        0x02,
        0x20,
        0x40
    };

    cpu.load_program(program, 0x1000);

    cpu.cycle();

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x1006
    );
}

TEST(CPU, CycleExecutesMovImmediate) {
    CPU cpu;

    const std::array<Byte, 5> program{
        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::A),
        0x01,
        42
    };

    cpu.load_program(program, 0x1000);

    cpu.cycle();

    EXPECT_EQ(
        cpu.state().accumulator(),
        42
    );

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x1005
    );
}

TEST(CPU, CycleExecutesTwoInstructions) {
    CPU cpu;

    const std::array<Byte, 10> program{
        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::A),
        0x01,
        10,

        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::R1),
        0x01,
        20
    };

    cpu.load_program(program, 0x1000);

    cpu.cycle();

    EXPECT_EQ(cpu.state().accumulator(), 10);
    EXPECT_EQ(cpu.state().program_counter(), 0x1005);

    cpu.cycle();

    EXPECT_EQ(
        cpu.state().register_value(1),
        20
    );

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x100A
    );
}

TEST(CPU, CycleExecutesJump) {
    CPU cpu;

    const std::array<Byte, 4> program{
        to_byte(Opcode::JMP),
        0x02,
        0x40,
        0x20
    };

    cpu.load_program(program, 0x1000);

    cpu.cycle();

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x4020
    );
}

TEST(CPU, CycleExecutesConditionalBranch) {
    CPU cpu;

    const std::array<Byte, 4> program{
        to_byte(Opcode::JZ),
        0x02,
        0x30,
        0x00
    };

    cpu.load_program(program, 0x1000);

    cpu.state().flags().set_zero(true);

    cpu.cycle();

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x3000
    );
}

TEST(CPU, CycleDoesNotTakeConditionalBranchWhenFlagIsClear) {
    CPU cpu;

    const std::array<Byte, 4> program{
        to_byte(Opcode::JZ),
        0x02,
        0x30,
        0x00
    };

    cpu.load_program(program, 0x1000);

    cpu.state().flags().set_zero(false);

    cpu.cycle();

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x1004
    );
}

TEST(CPU, InstructionRegisterContainsFetchedOpcode) {
    CPU cpu;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    cpu.load_program(program, 0x2000);

    cpu.cycle();

    EXPECT_EQ(
        cpu.state().instruction_register(),
        to_byte(Opcode::NOP)
    );
}

TEST(CPU, ProgramCanStartAtArbitraryAddress) {
    CPU cpu;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    cpu.load_program(program, 0x4321);

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x4321
    );

    cpu.cycle();

    EXPECT_EQ(
        cpu.state().program_counter(),
        0x4322
    );
}

TEST(CPU, RejectsProgramThatDoesNotFitInMemory) {
    CPU cpu;

    const std::array<Byte, 2> program{
        to_byte(Opcode::NOP),
        to_byte(Opcode::NOP)
    };

    EXPECT_THROW(
        cpu.load_program(program, 0xFFFF),
        std::runtime_error
    );
}
TEST(CPU, CycleReturnsExecutedInstruction) {
    CPU cpu;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    cpu.load_program(program, 0x1000);

    const Instruction instruction =
        cpu.cycle();

    EXPECT_EQ(
        instruction.opcode(),
        Opcode::NOP
    );
}