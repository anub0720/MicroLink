#include <array>
#include <gtest/gtest.h>

#include "microlink/runtime/runtime.hpp"

using namespace microlink::cpu;
using namespace microlink::runtime;

TEST(Runtime, StartsWithZeroCycles) {
    Runtime runtime;

    EXPECT_EQ(runtime.cycles_executed(), 0);
    EXPECT_FALSE(runtime.halted());
}

TEST(Runtime, StepExecutesOneInstruction) {
    Runtime runtime;

    const std::array<Byte, 5> program{
        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::A),
        0x01,
        42
    };

    runtime.load_program(program, 0x1000);

    runtime.step();

    EXPECT_EQ(
        runtime.cpu().state().accumulator(),
        42
    );

    EXPECT_EQ(
        runtime.cycles_executed(),
        1
    );
}

TEST(Runtime, RunExecutesRequestedNumberOfCycles) {
    Runtime runtime;

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

    runtime.load_program(program, 0x1000);

    const std::size_t executed =
        runtime.run(2);

    EXPECT_EQ(executed, 2);
    EXPECT_EQ(runtime.cycles_executed(), 2);

    EXPECT_EQ(
        runtime.cpu().state().accumulator(),
        10
    );

    EXPECT_EQ(
        runtime.cpu().state().register_value(1),
        20
    );
}

TEST(Runtime, RunStopsAtCycleLimit) {
    Runtime runtime;

    const std::array<Byte, 100> program = [] {
        std::array<Byte, 100> bytes{};
        bytes.fill(to_byte(Opcode::NOP));
        return bytes;
    }();

    runtime.load_program(program, 0x1000);

    const std::size_t executed =
        runtime.run(100);

    EXPECT_EQ(executed, 100);
    EXPECT_EQ(runtime.cycles_executed(), 100);
}

TEST(Runtime, ResetClearsCycleCount) {
    Runtime runtime;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    runtime.load_program(program, 0x1000);
    runtime.step();

    runtime.reset();

    EXPECT_EQ(runtime.cycles_executed(), 0);
    EXPECT_EQ(
        runtime.cpu().state().program_counter(),
        0
    );
}
TEST(Runtime, StepCreatesExecutionTrace) {
    Runtime runtime;

    const std::array<Byte, 5> program{
        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::A),
        0x01,
        42
    };

    runtime.load_program(program, 0x1000);

    runtime.step();

    ASSERT_EQ(runtime.trace().size(), 1);

    const ExecutionTrace& event =
        runtime.trace().front();

    EXPECT_EQ(event.cycle, 1);
    EXPECT_EQ(event.program_counter_before, 0x1000);
    EXPECT_EQ(event.program_counter_after, 0x1005);
    EXPECT_EQ(event.accumulator_before, 0);
    EXPECT_EQ(event.accumulator_after, 42);
    EXPECT_EQ(
        event.instruction.opcode(),
        Opcode::MOV
    );
}

TEST(Runtime, TraceCanBeCleared) {
    Runtime runtime;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    runtime.load_program(program, 0x1000);

    runtime.step();

    ASSERT_EQ(runtime.trace().size(), 1);

    runtime.clear_trace();

    EXPECT_TRUE(runtime.trace().empty());
}