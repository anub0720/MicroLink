#include <gtest/gtest.h>

#include "microlink/runtime/trace.hpp"

using namespace microlink::cpu;
using namespace microlink::runtime;

TEST(ExecutionTrace, StoresCycleInformation) {
    ExecutionTrace trace;

    trace.cycle = 7;
    trace.program_counter_before = 0x1000;
    trace.program_counter_after = 0x1005;
    trace.accumulator_before = 10;
    trace.accumulator_after = 42;

    EXPECT_EQ(trace.cycle, 7);
    EXPECT_EQ(trace.program_counter_before, 0x1000);
    EXPECT_EQ(trace.program_counter_after, 0x1005);
    EXPECT_EQ(trace.accumulator_before, 10);
    EXPECT_EQ(trace.accumulator_after, 42);
}

TEST(ExecutionTrace, StoresInstruction) {
    ExecutionTrace trace{
        .cycle = 1,
        .program_counter_before = 0x2000,
        .program_counter_after = 0x2005,
        .instruction = Instruction(
            Opcode::NOP
        )
    };

    EXPECT_EQ(
        trace.instruction.opcode(),
        Opcode::NOP
    );
}