#include <array>
#include <gtest/gtest.h>

#include "microlink/application/application.hpp"

using namespace microlink::cpu;
using namespace microlink::application;

TEST(ApiApplication, StateCanBeRead) {
    Application application;

    EXPECT_EQ(
        application.cpu_state().program_counter(),
        0
    );

    EXPECT_EQ(
        application.cpu_state().accumulator(),
        0
    );
}

TEST(ApiApplication, StepChangesExecutionState) {
    Application application;

    const std::array<Byte, 5> program{
        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::A),
        0x01,
        42
    };

    application.load_program(
        program,
        0x1000
    );

    application.step();

    EXPECT_EQ(
        application.cpu_state().accumulator(),
        42
    );

    EXPECT_EQ(
        application.trace().size(),
        1
    );
}

TEST(ApiApplication, ResetClearsState) {
    Application application;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    application.load_program(
        program,
        0x1000
    );

    application.step();
    application.reset();

    EXPECT_EQ(
        application.cpu_state().program_counter(),
        0
    );

    EXPECT_TRUE(
        application.trace().empty()
    );
}